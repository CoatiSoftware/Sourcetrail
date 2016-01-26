Remove self-healing on project file opened
	Open .msi in Orca
	Component table
	Remove key path of component with ID {31DB780A-8AA4-1A96-F39E-1BD7CFCC2418}*
	
	*I don't know if this id will stay the same, so here's how to find it:
		Just install Coati as is
		Open windows Event Viewer
		Open .coatiproject via file association (wont work properly, that's ok)
		Look for Application log
		Look for a Warning entry involving MsiInstaller at time of step 3
		Retrieve component id from message of that entry