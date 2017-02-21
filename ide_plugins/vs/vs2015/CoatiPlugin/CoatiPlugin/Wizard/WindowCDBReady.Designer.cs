namespace CoatiSoftware.CoatiPlugin.Wizard
{
    partial class WindowCDBReady
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(WindowCDBReady));
            this.label_message = new System.Windows.Forms.Label();
            this.button_ok = new System.Windows.Forms.Button();
            this.button_import = new System.Windows.Forms.Button();
            this.button_open = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label_message
            // 
            this.label_message.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label_message.Location = new System.Drawing.Point(10, 9);
            this.label_message.MinimumSize = new System.Drawing.Size(290, 70);
            this.label_message.Name = "label_message";
            this.label_message.Size = new System.Drawing.Size(290, 70);
            this.label_message.TabIndex = 0;
            this.label_message.Text = "Message";
            // 
            // button_ok
            // 
            this.button_ok.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.button_ok.Location = new System.Drawing.Point(12, 87);
            this.button_ok.Name = "button_ok";
            this.button_ok.Size = new System.Drawing.Size(75, 23);
            this.button_ok.TabIndex = 1;
            this.button_ok.Text = "Finish";
            this.button_ok.UseVisualStyleBackColor = true;
            this.button_ok.Click += new System.EventHandler(this.button_ok_Click);
            // 
            // button_import
            // 
            this.button_import.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.button_import.Location = new System.Drawing.Point(227, 87);
            this.button_import.Name = "button_import";
            this.button_import.Size = new System.Drawing.Size(75, 23);
            this.button_import.TabIndex = 2;
            this.button_import.Text = "Import";
            this.button_import.UseVisualStyleBackColor = true;
            this.button_import.Click += new System.EventHandler(this.button_import_Click);
            // 
            // button_open
            // 
            this.button_open.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.button_open.Location = new System.Drawing.Point(111, 87);
            this.button_open.Name = "button_open";
            this.button_open.Size = new System.Drawing.Size(92, 23);
            this.button_open.TabIndex = 3;
            this.button_open.Text = "Open Folder";
            this.button_open.UseVisualStyleBackColor = true;
            this.button_open.Click += new System.EventHandler(this.button_open_Click);
            // 
            // WindowCDBReady
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(314, 122);
            this.ControlBox = false;
            this.Controls.Add(this.button_open);
            this.Controls.Add(this.button_import);
            this.Controls.Add(this.button_ok);
            this.Controls.Add(this.label_message);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MinimumSize = new System.Drawing.Size(330, 160);
            this.Name = "WindowCDBReady";
            this.Text = "CDB Ready";
            this.Resize += new System.EventHandler(this.WindowCDBReady_Resize);
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.Button button_ok;
        private System.Windows.Forms.Button button_import;
        private System.Windows.Forms.Button button_open;
        private System.Windows.Forms.Label label_message;
    }
}