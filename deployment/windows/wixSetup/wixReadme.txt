-------------------
Build the installer
-------------------

-Prerequisites
	-configure for build
		-make sure the Visual Studio IDE directory is in you OS path variable ('../Microsoft Visual Studio14/Common7/IDE/')
	-Install the WiX toolset...
		- http://wixtoolset.org/releases/
		-I used stable version 3.10.3 for development
		-make sure the WiX directory is in you OS path variable ('..\WiX Toolset v3.10\bin')
	-Sourcetrail has to be built with the 'deploy' flag
		-installer would still build if the deploy flag wasnt set, but the Sourcetrail wont run properly on a user machine

-Execute build.bat
	-This will build sourcetrail.msi file, all needed custom action dlls and the setup.exe file
	-setup.exe and sourcetrail.msi are needed to install
		-setup.exe checks whether sourcetrail is installed and will either start the initial installation or the upgrade installation


---------------------
Clean the project
---------------------
-Just delete the "bin" and "build" folders


---------------------
Update Version Number
---------------------
-See below ('chapter' Create new Version)
		
		
---------------------------------------------------
Add new components (e.g. files) to the installation
---------------------------------------------------

[description]
-xml elements are described as follows:
	-To do X use tag <tag>
		-attribute: attribute description
		-... 

		
Generel
	-to add a file create a <Component> element
		-mind the <Directory> tags, they define where the component will be installed. Add new directories if necessary
			-Id: the directory will be refered to within the wix project using this id
			-Name: the name of the folder that will be created on the user machine
		-Id: is used to refer to the component within the wix project
		-Guid: GUIDs are unique id's used by windows to identify and refer to installed files, applications and so on. You need to create a new GUID for every new component
				Visual Studio can create GUIDs (Tools->Create GUID), or just google for a tool
				It's reasonably safe to assume that every newly created GUID is unique
		-Add a file to component using the <File> tag
			-Id: guess what...
			-Name: the name that the file will have on the user machine, may be different from the file name on the source machine
			-Source: the path and name to the file on the source machine, relative to the current .wxs file
			-KeyPath: depends on where you want to install the file, see below in 'Add new file to...'
		-usually there is one component for each single file. Multiple files can be added to a component, this might make sense if the files are considered 100% unseperable. The recommended way however is one file per component
			
			
Add new file to the installation directory (installDir.wxs)
	-static files that must not be changed go here
	-just like above, with KeyPath='yes'
		-if there are more files in a component, only one file has the KeyPath attribute
	

Add new file to the user folder (appDataDir.wxs)
	-files that will be changed by sourcetrail or the user should be put here
	-needs a RegistryKey element within the component element to provide the KeyPath
		- <RegistryKey Action="none" Key="Software\[Manufacturer]\[ProductName]\>yourKeyNameHere<" Root="HKCU" >
				<RegistryValue Type="integer" Value="1" KeyPath="yes" />
			</RegistryKey>
	

Not quite done yet! (sourcetrail.wxs)
	-components need to be added to a feature (a feature is part of a software, consisting of components, that may or may not be installed during setup)
		-right now we only have the Complete/Program feature, so no part of sourcetrail is optional right now
	-to add a component to a feature use the <ComponentRef> tag
		-Id: id of the component you want to refere to
	

------------------
Create new version
------------------

Version Number
	-is now a preprocessor variable
	-is defined in build.bat
		-candle.exe -dprojectVersion="0.7.0" ...
		-replace 0.7.0 with the desired version number

Minor upgrage (sourcetrail.wxs)
	-for when only a few files are to be updated
	-update the version number in build.bat script
	-do NOT change the product GUID
	-the package GUID has to change, that happens automatically though so don't worry 'bout it
	

Major upgrade (sourcetrail.wxs)
	-for big changes, like a new major version
	-update the version number in build.bat script
	-DO change the product GUID
	-insert the same GUID into the uninstall_wix.bat script
	-package GUID changes too, but thats again done automatically