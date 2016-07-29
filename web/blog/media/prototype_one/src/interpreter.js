SHIFT.Interpreter = {
	visibility : SHIFT.Visibility.None,
	abstraction : SHIFT.Abstraction.None,

	CreateProgram : function( data ) {
		var program = new SHIFT.Program();

		for ( var i = 0; i < data.length; i++ ) {
			this.ParseNode( data[i], program, program );
		}
		return program;
	},

	ParseNode : function( node, program, namespace ) {
		if ( !this[node.type] ) {
			console.error( 'Unknown node type', node.type, node );
			return;
		}

		this[node.type]( node, program, namespace );
	},

	ParseContent : function( content, program, namespace ) {
		if ( !content ) {
			return;
		}

		for ( var i = 0; i < content.length; i++ ) {
			this.ParseNode( content[i], program, namespace );
		}
	},

	file : function( node, program, namespace ) {
		this.fileName = node.name.split( '.' )[0];
		this.ParseContent( node.content, program, namespace );
	},

	class : function( node, program, namespace ) {
		var classModel = namespace.CreateClass( node.name, this.CreateLocation( node.line ), this.visibility );

		if ( node.base ) {
			program.AddInheritance( program.GetClass( node.base ), classModel, this.CreateLocation( node.line ) );
		}

		if ( node.content ) {
			var visibility = this.visibility;

			if ( node.content.public ) {
				this.visibility = SHIFT.Visibility.Public;
				this.ParseContent( node.content.public, program, classModel );
			}

			if ( node.content.private ) {
				this.visibility = SHIFT.Visibility.Private;
				this.ParseContent( node.content.private, program, classModel );
			}

			this.visibility = visibility;
		}
	},

	member : function( node, program, namespace ) {
		var member = namespace.CreateMember( node.name, this.CreateLocation( node.line ), this.visibility, this.abstraction );
		if ( node.class ) {
			member.typename = node.class;
			program.AddInstantiation( member, program.GetClass( node.class ), this.CreateLocation( node.line ) );
		}
		if ( node.abstraction ) {
			this.AddAbstraction( member, node.abstraction );
		}
	},

	method : function( node, program, namespace ) {
		var methodModel = namespace.CreateMethod( node.name, this.CreateLocation( node.line ), this.visibility, this.abstraction );
		this.AddParams( methodModel, node, program );
		if ( node.abstraction ) {
			this.AddAbstraction( methodModel, node.abstraction );
		}
		this.ParseContent( node.content, program, methodModel );
	},

	AddAbstraction : function( obj, abstraction ) {
		switch ( abstraction ) {
			case 'static' : obj.SetAbstraction( SHIFT.Abstraction.Static ); break;
			case 'virtual' : obj.SetAbstraction( SHIFT.Abstraction.Virtual ); break;
			case 'pure_virtual' : obj.SetAbstraction( SHIFT.Abstraction.PureVirtual ); break;
			default : console.error( 'Abstraction unknown', abstraction );
		}
	},

	use : function( node, program, namespace ) {
		if ( node.class && node.member ) {
			program.AddUse( namespace, program.GetClass( node.class ).GetMember( node.member ), this.CreateLocation( node.line ) );
		} else if ( node.member ) {
			program.AddUse( namespace, namespace.parent.GetMember( node.member ), this.CreateLocation( node.line ) );
		} else {
			program.AddUse( namespace, program.GetClass( node.class ), this.CreateLocation( node.line ) );
		}
	},

	call : function( node, program, namespace ) {
		if ( node.class ) {
			program.AddCall( namespace, program.GetClass( node.class ).GetMethod( node.method ), this.CreateLocation( node.line ) );
		} else if ( node.function ) {
			program.AddCall( namespace, program.GetFunction( node.function ), this.CreateLocation( node.line ) );
		} else {
			program.AddCall( namespace, namespace.parent.GetMethod( node.method ), this.CreateLocation( node.line ) );
		}
	},

	function : function( node, program, namespace ) {
		var functionModel = program.CreateFunction( node.name, this.CreateLocation( node.line ) );
		this.AddParams( functionModel, node, program );
		this.ParseContent( node.content, program, functionModel );
	},

	CreateLocation : function( lineNumber ) {
		return new SHIFT.Location( this.fileName, lineNumber );
	},

	AddParams : function( functionModel, node, program ) {
		if ( node.params ) {
			for ( var i = 0; i < node.params.length; i++ ) {
				program.AddUse( functionModel, program.GetClass( node.params[i] ), this.CreateLocation( node.line ) );
			}
			functionModel.params = node.params;
		}

		if ( node.return ) {
			program.AddUse( functionModel, program.GetClass( node.return ), this.CreateLocation( node.line ) );
			functionModel.return = node.return;
		}
	}
};
