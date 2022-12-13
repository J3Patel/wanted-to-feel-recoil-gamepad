using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using GTA;
using GTA.Math;
using System.Windows.Forms;
using System.Drawing;
using GTA.Native;
using System.IO.Ports;
using System.Threading;

using System.IO;
using System.Globalization;


namespace GTA_MACHINE_GUN
{
    public class attacker : Script
    {
        List<string> weaponNames = new List<string>();
        List<double> rofs = new List<double>();
        static SerialPort _serialPort;
        
        private UIText logger;
        private UIText logger2;
        private Vector3 _lastHitPos = Vector3.Zero;
        private Vector3 newHitPos = Vector3.Zero;
        private int _lastSentData = 1204;
        private int calDelay = 1000;
        private Thread serialThread;
        private bool isLoaded = false;
        public attacker() {
            
            this.Tick += onTick;
            this.KeyDown += onKeyDown;
            this.KeyUp += onKeyUp;

          //  Interval = 1;

            logger = new UIText("TAKE", new Point(900, 600), 0.8f, Color.White, GTA.Font.ChaletLondon, true, true, true);
            logger.Color = Color.Red;
            logger.Caption = "Hi";
            logger.Enabled = true;
            

            logger2 = new UIText("TAKE", new Point(900, 500), 0.8f, Color.White, GTA.Font.ChaletLondon, true, true, true);
            logger2.Color = Color.Green;
            logger2.Caption = "Hi";
            logger2.Enabled = true;
            logger2.Draw();


            _serialPort = new SerialPort();
            _serialPort.PortName = "COM17"; //Set your board COM
            _serialPort.BaudRate = 115200;
            _serialPort.Open();

            loadRMPCSV();
            serialThread = new Thread(() =>
            {
                while (true)
                {
                  
                      
                  }

                    
            });
            serialThread.Priority= ThreadPriority.Highest;
           //  serialThread.IsBackground = true;
             serialThread.Start();
        }

        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);
            serialThread.Suspend();
        }

        int hit = 0;

        private void onTick(object sender, EventArgs e)
        {

            
            Ped playerPed = Game.Player.Character;

            var weaponHash = Function.Call<uint>(Hash.GET_SELECTED_PED_WEAPON, playerPed);

            var wh = (WeaponHash)weaponHash;
            var sel = Game.GetGXTEntry(Weapon.GetDisplayNameFromHash(wh));
            gunSelect(sel);

            var out1 = new OutputArgument();
            Function.Call(Hash.GET_PED_LAST_WEAPON_IMPACT_COORD, Game.Player.Character, out1);
            newHitPos = out1.GetResult<Vector3>();

            if (_lastHitPos != newHitPos)
            {
                _lastHitPos = newHitPos;
                sendDataToArduino(1);
                //logger2.Caption = "FIRES";
                //Thread.Sleep(calDelay);
            }
            else
            {
                // logger2.Caption = "DELAYED";
                sendDataToArduino(0);
                // Thread.Sleep(calDelay);
            }


            logger.Draw();
            logger2.Draw();
            isLoaded = true;

        }

        private void sendDataToArduino(int fire)
        {
            
            if (_lastSentData !=fire)
            {
               
                _lastSentData = fire;
               
                    _serialPort.WriteLine(fire.ToString());
                    logger2.Caption = fire.ToString();
               
            }

        }

        private void sendRPMToArduino(int fire)
        {

                _serialPort.WriteLine("r" + fire.ToString());
                logger2.Caption = fire.ToString();

        }

        private void onKeyUp(object sender, KeyEventArgs e)
        {
            
        }
        private void onKeyDown(object sender, KeyEventArgs e)
        {

       
        }

        private void loadRMPCSV()
        {
            weaponNames.Clear();
            rofs.Clear();
            string[] csvLines = System.IO.File.ReadAllLines(@"J:\gta_weapon.csv");

            
            for (int i = 1; i < csvLines.Length; i++)
            {
                string[] rowData = csvLines[i].Split(',');

                weaponNames.Add(rowData[1]);
                
                double g = Convert.ToDouble(rowData[6]);
                rofs.Add(g);
            }

            logger.Caption = weaponNames[0] + "  " + rofs[0].ToString();
            
        }

        string _prevGun = "";
        private void gunSelect(string name)
        {

            if(_prevGun != name)
            {
                _prevGun = name;
                var found = "not found";
                var rof = "NO";
                for (int i = 0; i < weaponNames.Count; i++)
                {
                    if (weaponNames[i].ToLower() == name.ToLower())
                    {
                        found = weaponNames[i];
                        rof = rofs[i].ToString();
                        var rpm = Convert.ToInt32(rofs[i]);
                        calDelay = 60000 / (2 * rpm);
                       if(calDelay - 20 > 10 ) {
                            calDelay = calDelay - 10;
                                 
                        }
                        sendRPMToArduino(calDelay);
                        break;
                    }
                }

                logger.Caption = name + "   " + found + "  " + calDelay;
            }

            
        }
    }

    
}

