using System;
using System.Drawing;
using System.Windows.Forms;

namespace CoatiSoftware.SourcetrailPlugin.Wizard
{
	public partial class WindowCdbReady : Form
	{
		private WindowCreateCdb.CreationResult _creationResult = new WindowCreateCdb.CreationResult();

		public WindowCdbReady()
		{
			InitializeComponent();

			label_message.AutoSize = false;
			label_message.MaximumSize = new Size((int)((float)MaximumSize.Width * 0.8f), 0);
		}

		public void setData(WindowCreateCdb.CreationResult creationResult)
		{
			_creationResult = creationResult;

			label_message.Text = "The Compilation Database \"" + creationResult._cdbName + 
				"\" was created at directory \"" + creationResult._cdbDirectory + "\".\n" + 
				"Do you want to auto-import it in Sourcetrail now?";
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

		private void WindowCdbReady_Resize(object sender, EventArgs e)
		{
			label_message.MaximumSize = new Size((int)((float)MaximumSize.Width * 0.8f), 0);
		}
	}
}
