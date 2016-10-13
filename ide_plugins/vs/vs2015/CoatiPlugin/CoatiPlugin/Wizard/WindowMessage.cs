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
    public partial class WindowMessage : Form
    {
        private string m_title = "Title";
        private string m_message = "Message";

        public delegate void Callback();

        private Callback m_onOK = null;
        private Callback m_onCancel = null;

        public string Title
        {
            get { return m_title; }
            set { m_title = value; }
        }

        public string Message
        {
            get { return m_message; }
            set { m_message = value; }
        }

        public Callback OnOK
        {
            get { return m_onOK; }
            set { m_onOK = value; }
        }

        public Callback OnCancel
        {
            get { return m_onCancel; }
            set { m_onCancel = value; }
        }

        public WindowMessage()
        {
            InitializeComponent();
        }

        public void RefreshWindow()
        {
            Text = m_title;
            labelContent.Text = m_message;

            //if(m_onOK != null)
            //{
            //    buttonOK.Show();
            //}
            //else
            //{
            //    buttonOK.Hide();
            //}

            if(m_onCancel != null)
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
            if(m_onCancel != null)
            {
                m_onCancel();
            }

            Close();
        }

        private void buttonOK_Click(object sender, EventArgs e)
        {
            if(m_onOK != null)
            {
                m_onOK();
            }

            Close();
        }
    }
}
