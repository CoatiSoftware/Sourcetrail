using System;
using System.Windows.Forms;

namespace CoatiSoftware.SourcetrailPlugin.Wizard
{
    public partial class WindowMessage : Form
    {
        private string _title = "Title";
        private string _message = "Message";

        public delegate void Callback();

        private Callback _onOK = null;
        private Callback _onCancel = null;

        public string Title
        {
            get { return _title; }
            set { _title = value; }
        }

        public string Message
        {
            get { return _message; }
            set { _message = value; }
        }

        public Callback OnOK
        {
            get { return _onOK; }
            set { _onOK = value; }
        }

        public Callback OnCancel
        {
            get { return _onCancel; }
            set { _onCancel = value; }
        }

        public WindowMessage()
        {
            InitializeComponent();
        }

        public void RefreshWindow()
        {
            Text = _title;
            labelContent.Text = _message;

            if(_onCancel != null)
            {
                buttonCancel.Show();
            }
            else
            {
                buttonCancel.Hide();
            }
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            if(_onCancel != null)
            {
                _onCancel();
            }

            Close();
        }

        private void buttonOK_Click(object sender, EventArgs e)
        {
            if(_onOK != null)
            {
                _onOK();
            }

            Close();
        }
    }
}
