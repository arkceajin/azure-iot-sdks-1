﻿namespace e2e_ss
{
    using Microsoft.Azure.Devices;
    using System;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.Text;
    using System.Threading;
    using System.Threading.Tasks;
    using SysTimer = System.Timers.Timer;

    class Program
    {
        const double OneSecond = 1000;

        static string             _cs;
        static JobClient          _dj;
        static RegistryManager    _rm;
        static ManualResetEvent   _clientReady = new ManualResetEvent(false);
        static string             _deviceId = "SimpleSample_" + Guid.NewGuid().ToString();
        static bool               _registered = false;
        static SysTimer           _closer;

        static Dictionary<String, TestCase> _ReadTestCases = new Dictionary<string, TestCase>();
        static Dictionary<String, TestCase> _WriteTestCases = new Dictionary<string, TestCase>();
        static Dictionary<String, TestCase> _ExecuteTestCases = new Dictionary<string, TestCase>();
        static Dictionary<String, TestCase> _ObserveTestCases = new Dictionary<string, TestCase>();


        static void ListAllSimpleSampleTestDevices()
        {
            Task<IEnumerable<Device>> results = _rm.GetDevicesAsync(500);
            results.Wait();
            var devices = results.Result;

            foreach (var device in devices)
            {
                Console.WriteLine("*device: {0}", device.Id);
            }
        }


        static void UnregisterStaleTestDevices()
        {
            Task<IEnumerable<Device>> results = _rm.GetDevicesAsync(500);
            results.Wait();
            var devices = results.Result;

            foreach (var device in devices)
            {
                if (device.Id.StartsWith("SimpleSample_"))
                {
                    Unregister(device);
                }
            }
        }


        static void Unregister(Device device)
        {
            try
            {
                _rm.RemoveDeviceAsync(device).Wait();
                Console.WriteLine("*Unregistered device: {0}", device.Id);
            }

            catch (Exception x)
            {
                Console.WriteLine("Exception: {0}", x.ToString());
            }
        }


        static void Unregister()
        {
            Task<Device> getDeviceTask = _rm.GetDeviceAsync(_deviceId);
            getDeviceTask.Wait();

            Device device = getDeviceTask.Result;
            Unregister(device);
        }


        static String Register()
        {
            Task<Device> newDevice = _rm.AddDeviceAsync(new Device(_deviceId));
            newDevice.Wait();
            var device = newDevice.Result;

            Console.WriteLine("Registered device: {0}", device.Id);

            string hostName = _cs.Split(new string[] { ";" }, StringSplitOptions.None)[0];
            StringBuilder deviceCreds = new StringBuilder();
            deviceCreds.Append(String.Format("{0};DeviceId={1};SharedAccessKey={2}{3}", hostName, device.Id,
                device.Authentication.SymmetricKey.PrimaryKey, System.Environment.NewLine));
            return deviceCreds.ToString(0, deviceCreds.Length - 2);
        }


        static String GetProperty(String property)
        {
            Task<Device> getDeviceTask = _rm.GetDeviceAsync(_deviceId);
            getDeviceTask.Wait();

            Device device = getDeviceTask.Result;

            if (device.SystemProperties.ContainsKey(property))
            {
                var value = device.SystemProperties[property].Value.ToString();
                return value;
            }

            Console.WriteLine("{0} not found in SystemProperties", property);
            return String.Empty;
        }


        private static void onDataOut(object caller, DataReceivedEventArgs eArgs)
        {
            if ((string.IsNullOrEmpty(eArgs.Data) == false) && eArgs.Data.StartsWith("Info: "))
            {
                if (eArgs.Data.Contains("REGISTERED"))
                {
                    _registered = true;
                    Console.WriteLine("*Device connected to DM channel");
                }

                else
                {
                    string[] data = eArgs.Data.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
                    if (data.Length > 2)
                    {
                        string propertyName = data[data.Length - 1];
                        if (_ReadTestCases.ContainsKey(propertyName))
                        {
                            // looking for: 'Info: returning %d/%s for <PropertyName>'
                            if (data[1].Equals("returning"))
                            {
                                _ReadTestCases[propertyName].ExpectedValue = data[2];
                            }

                            else if (data[1].Equals("being") && data[2].Equals("set"))
                            {
                                _ReadTestCases[propertyName].RecordedValue = data[2];
                            }

                            else if (data[1].Equals("inside") && data[2].Equals("execute"))
                            {
                                _ReadTestCases[propertyName].RecordedValue = "true";
                            }
                        }
                    }
                }
            }

            /** If all test cases have been run */
            bool done = true;
            foreach (var aCase in _ReadTestCases)
            {
                done &= aCase.Value.HasRun;
                if (done == false) break;
            }

            foreach (var aCase in _WriteTestCases)
            {
                done &= aCase.Value.HasRun;
                if (done == false) break;
            }

            foreach (var aCase in _ExecuteTestCases)
            {
                done &= aCase.Value.HasRun;
                if (done == false) break;
            }

            foreach (var aCase in _ObserveTestCases)
            {
                done &= aCase.Value.HasRun;
                if (done == false) break;
            }

            if (done)
            {
                _clientReady.Set();
            }
        }


        private static void onErrorOut(object caller, DataReceivedEventArgs eArgs)
        {
            if (string.IsNullOrEmpty(eArgs.Data) == false)
            {
                if (eArgs.Data.StartsWith("Error:") && eArgs.Data.Contains("Registration FAILED"))
                {
                    _registered = false;
                    Console.WriteLine("*Device failed to connect to DM channel");

                    _clientReady.Set();
                }

                else if (eArgs.Data.StartsWith("Observe Update"))
                {
                    string[] data = eArgs.Data.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
                    string Uri = data[1].Substring(7, data[1].Length - 9);

                    if (_ObserveTestCases.ContainsKey(Uri))
                    {
                        TestCase aCase = _ObserveTestCases[Uri];
                        if (aCase != null)
                        {
                            int size = Int32.Parse(data[2]);
                            aCase.ExpectedValue = data[3].Substring(0, size);

                            var checkServiceValue = new SysTimer(6000);
                            checkServiceValue.Elapsed += (sender, e) =>
                            {
                                aCase.RecordedValue = GetProperty(aCase.Name);
                            };

                            checkServiceValue.AutoReset = false;
                            checkServiceValue.Start();
                        }
                    }
                }
            }
        }


        class DmClient
        {
            private Process process_;

            private DmClient(String exeName, String connectionString)
            {
                var startInfo = new ProcessStartInfo()
                {
                    FileName = exeName,
                    CreateNoWindow = true,
                    UseShellExecute = false,
                    Arguments = connectionString,
                    RedirectStandardError = true,
                    RedirectStandardOutput = true
                };

                // can throw file not found exception
                process_ = Process.Start(startInfo);
                if (process_ != null)
                {
                    process_.EnableRaisingEvents = true;
                    process_.OutputDataReceived += new DataReceivedEventHandler(onDataOut);
                    process_.ErrorDataReceived += new DataReceivedEventHandler(onErrorOut);

                    process_.BeginOutputReadLine();
                    process_.BeginErrorReadLine();
                }
            }


            public static DmClient Start(String exeName, String connectionString)
            {
                var dmClient = new DmClient(exeName, connectionString);
                return (dmClient.process_ == null) ? null : dmClient;
            }


            public void Stop()
            {
                try
                {
                    process_.Kill();
                    process_.WaitForExit();
                }

                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }
        }


        private class TestCase
        {
            public enum TestType { Read, Write, Observe, Execute };

            public String Name { get; }

            private TestType type_;
            public TestType Type
            {
                get { return type_; }
                private set
                {
                    this.type_ = value;
                    if (value == TestType.Execute)
                    {
                        this.ExpectedValue = "true";
                    }
                }
            }

            public String  RecordedValue { get; set; }
            public String ExpectedValue { get; set; }

            public TestCase(String name, TestType type)
            {
                this.Name = name;
                this.Type = type;
            }

            public bool IsValid
            {
                get { return AreEqual(ExpectedValue, RecordedValue, Name); }
            }

            public bool HasRun
            {
                get { return ((this.ExpectedValue != null) && (this.RecordedValue != null)); }
            }
        }


        static void RunTestCases()
        {
            try
            {
                _dj = JobClient.CreateFromConnectionString(_cs);

                while (_registered == false)
                    Thread.Sleep(1000);

                foreach (var one in _ReadTestCases)
                {
                    one.Value.RecordedValue = ReadPropertyThroughService(one.Value.Name);
                }

                foreach (var one in _WriteTestCases)
                {
                    WritePropertyThroughService(one.Value.Name, one.Value.ExpectedValue);
                }

                foreach (var one in _ExecuteTestCases)
                {
                    ExecuteResourceThroughService(one.Value.Name);
                }
            }

            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }


        /**
         *  arg[0] = full path to iotdm_simple_sample binary.
         *  arg[1] = IoT Hub connection string.
         */
        static int Main(string[] args)
        {
            if (args.Length != 2)
            {
                Console.WriteLine("Usage: ");
                Console.WriteLine("  - to run the tests: 2e_ss.exe <clientEXE> <IotHub Connection String>");
                Console.WriteLine("  - to delete stale test devices: 2e_ss.exe --cleanupRegistrar <IotHub Connection String>");

                return -1;
            }

            _cs = args[1];
            _rm = RegistryManager.CreateFromConnectionString(_cs);

            if (args[0].Equals("--cleanupRegistrar"))
            {
                UnregisterStaleTestDevices();

                return 0;
            }

            String connectionString = Register();
            var client = DmClient.Start(args[0], connectionString);

            //// the 'read' test cases.
            //var oneCase = new TestCase(SystemPropertyNames.FirmwareVersion, TestCase.TestType.Read);
            //_ReadTestCases.Add("Device_FirmwareVersion", oneCase);

            //// the 'write' test cases
            //oneCase = new TestCase(SystemPropertyNames.Timezone, TestCase.TestType.Write);
            //oneCase.ExpectedValue = "-10:00"; /* US/Hawaii timezone */
            //_WriteTestCases.Add("Device_Timezone", oneCase);

            //// the 'execute' test cases
            //oneCase = new TestCase("Device_FactoryReset", TestCase.TestType.Execute);
            //_ExecuteTestCases.Add("Device_FactoryReset", oneCase);
            //oneCase = new TestCase("Device_Reboot", TestCase.TestType.Execute);
            //_ExecuteTestCases.Add("Device_Reboot", oneCase);
            //oneCase = new TestCase("FirmwareUpdate_Update", TestCase.TestType.Execute);
            //_ExecuteTestCases.Add("FirmwareUpdate_Update", oneCase);

            // the 'observe' test cases
            /**
             * Note that we are using a URI - This is a side effect of wakaama messages on stderr...
             *    Let's figure out a more generic way of identifying the URI!
             */
            _ObserveTestCases.Add("/1/0/1", new TestCase(SystemPropertyNames.RegistrationLifetime, TestCase.TestType.Observe));
            _ObserveTestCases.Add("/1/0/2", new TestCase(SystemPropertyNames.DefaultMinPeriod, TestCase.TestType.Observe));
            _ObserveTestCases.Add("/1/0/3", new TestCase(SystemPropertyNames.DefaultMaxPeriod, TestCase.TestType.Observe));
            _ObserveTestCases.Add("/3/0/0", new TestCase(SystemPropertyNames.Manufacturer, TestCase.TestType.Observe));
            _ObserveTestCases.Add("/3/0/1", new TestCase(SystemPropertyNames.ModelNumber, TestCase.TestType.Observe));
            _ObserveTestCases.Add("/3/0/2", new TestCase(SystemPropertyNames.SerialNumber, TestCase.TestType.Observe));
            _ObserveTestCases.Add("/3/0/3", new TestCase(SystemPropertyNames.FirmwareVersion, TestCase.TestType.Observe));
            _ObserveTestCases.Add("/3/0/9", new TestCase(SystemPropertyNames.BatteryLevel, TestCase.TestType.Observe));
            _ObserveTestCases.Add("/3/0/10", new TestCase(SystemPropertyNames.MemoryFree, TestCase.TestType.Observe));
            _ObserveTestCases.Add("/3/0/13", new TestCase(SystemPropertyNames.CurrentTime, TestCase.TestType.Observe));
            _ObserveTestCases.Add("/3/0/14", new TestCase(SystemPropertyNames.UtcOffset, TestCase.TestType.Observe));
            _ObserveTestCases.Add("/3/0/15", new TestCase(SystemPropertyNames.Timezone, TestCase.TestType.Observe));
            _ObserveTestCases.Add("/3/0/17", new TestCase(SystemPropertyNames.DeviceDescription, TestCase.TestType.Observe));
            _ObserveTestCases.Add("/3/0/18", new TestCase(SystemPropertyNames.HardwareVersion, TestCase.TestType.Observe));
            _ObserveTestCases.Add("/3/0/20", new TestCase(SystemPropertyNames.BatteryStatus, TestCase.TestType.Observe));
            _ObserveTestCases.Add("/3/0/21", new TestCase(SystemPropertyNames.MemoryTotal, TestCase.TestType.Observe));
            _ObserveTestCases.Add("/5/0/3", new TestCase(SystemPropertyNames.FirmwareUpdateState, TestCase.TestType.Observe));
            _ObserveTestCases.Add("/5/0/5", new TestCase(SystemPropertyNames.FirmwareUpdateResult, TestCase.TestType.Observe));
            _ObserveTestCases.Add("/5/0/6", new TestCase(SystemPropertyNames.FirmwarePackageName, TestCase.TestType.Observe));
            _ObserveTestCases.Add("/5/0/7", new TestCase(SystemPropertyNames.FirmwarePackageVersion, TestCase.TestType.Observe));
            _ObserveTestCases.Add("/10241/0/1", new TestCase("ConfigurationName", TestCase.TestType.Observe));
            _ObserveTestCases.Add("/10241/0/2", new TestCase("ConfigurationValue", TestCase.TestType.Observe));

            Thread thread = new Thread(new ThreadStart(RunTestCases));
            thread.Start();

            /**
             *  Start a timer for the process. The interval should be large enough to validate all tests.
             *  Care must be taken to ensure PMIN and PMAX, for example, are honored.
             */
            _closer = new SysTimer(10 * 60 * OneSecond);
            _closer.Elapsed += (sender, e) =>
            {
                _clientReady.Set();
            };

            _closer.Start();

            _clientReady.WaitOne();
            if (_registered)
                client.Stop();

            /** report -- */

            thread.Abort();
            Unregister();

            return 0;
        }


        static bool AreEqual(string expected, string actual, string message)
        {
            if (!String.Equals(expected, actual))
            {
                Console.Write("ERROR: Expected '{0}' to equal '{1}'", expected, actual);
                Console.WriteLine(" {0}", (message == null) ? String.Empty : message);

                return false;
            }

            return true;
        }


        static string ReadPropertyThroughService(string propertyName)
        {
            try
            {
                var jobID = "Read" + propertyName + Guid.NewGuid().ToString();

                Console.WriteLine("ReadPropertyThroughService({0})", propertyName);
                Task<JobResponse> job = _dj.ScheduleSystemPropertyReadAsync(jobID, _deviceId, propertyName);
                job.Wait();
                JobResponse rs = job.Result;
                while (rs.Status < JobStatus.Completed)
                {
                    Thread.Sleep(2000);
                    job = _dj.GetJobAsync(jobID);
                    job.Wait();
                    rs = job.Result;
                }

                if (rs.Status == JobStatus.Completed)
                {
                    return GetProperty(propertyName);
                }
            }

            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }

            return String.Empty;
        }


        static bool WritePropertyThroughService(string propertyName, string propertyValue)
        {
            try
            {
                var jobID = "Write" + propertyName + Guid.NewGuid().ToString();

                Task<JobResponse> job = _dj.ScheduleSystemPropertyWriteAsync(jobID, _deviceId, propertyName, propertyValue);
                job.Wait();
                JobResponse rs = job.Result;
                while (rs.Status < JobStatus.Completed)
                {
                    Thread.Sleep(2000);
                    job = _dj.GetJobAsync(jobID);
                    job.Wait();
                    rs = job.Result;
                }

                return (rs.Status == JobStatus.Completed);
            }

            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }

            return false;
        }


        static bool ExecuteResourceThroughService(string resourceName)
        {
            try
            {
                var jobID = "Execute" + resourceName + Guid.NewGuid().ToString();

                Task<JobResponse> job = null;
                if (resourceName.Equals("Device_FactoryReset"))
                {
                    job = _dj.ScheduleFactoryResetDeviceAsync(jobID, _deviceId);
                }

                else if (resourceName.Equals("Device_Reboot"))
                {
                    job = _dj.ScheduleRebootDeviceAsync(jobID, _deviceId);
                }

                else if (resourceName.Equals("FirmwareUpdate_Update"))
                {
                    job = _dj.ScheduleFirmwareUpdateAsync(jobID, _deviceId, "https://FakeURI", TimeSpan.MinValue);
                }

                if (job != null)
                {
                    job.Wait();
                    JobResponse rs = job.Result;
                    while (rs.Status < JobStatus.Completed)
                    {
                        Thread.Sleep(2000);
                        job = _dj.GetJobAsync(jobID);
                        job.Wait();
                        rs = job.Result;
                    }

                    return (rs.Status == JobStatus.Completed);
                }
            }

            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }

            return false;
        }
    }
}
