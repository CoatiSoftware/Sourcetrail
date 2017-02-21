using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CoatiSoftware.CoatiPlugin.Wizard
{
    public partial class WindowCDBReady : Form
    {
        private string _message0 = "The CDB ";
        private string _message1 = " was created at directory ";
        private string _message2 = "Do you want to auto-import it in Coati now?";

        private WindowCreateCDB.CreationResult _creationResult = new WindowCreateCDB.CreationResult();

        public WindowCDBReady()
        {
            InitializeComponent();

            label_message.AutoSize = false;
            label_message.MaximumSize = new Size((int)((float)MaximumSize.Width * 0.8f), 0);
        }

        public void setData(WindowCreateCDB.CreationResult creationResult)
        {
            _creationResult = creationResult;

            label_message.Text = _message0 + "'" + creationResult._cdbName + "'" + _message1 + "\"" + creationResult._cdbDirectory + "\".";
            label_message.Text += "\n" + _message2;
        }

        private void button_ok_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void button_open_Click(object sender, EventArgs e)
        {
            Utility.SystemUtility.OpenWindowsExplorerAtDirectory(_creationResult._cdbDirectory);
        }

        private void button_import_Click(object sender, EventArgs e)
        {
            string message = Utility.NetworkProtocolUtility.CreateCreateProjectMessage(_creationResult._cdbDirectory + "\\" + _creationResult._cdbName + ".json", _creationResult._headerDirectories);

            Utility.AsynchronousClient.Send(message);

            Close();
        }

        private void WindowCDBReady_Resize(object sender, EventArgs e)
        {
            label_message.MaximumSize = new Size((int)((float)MaximumSize.Width * 0.8f), 0);
        }
    }
}
