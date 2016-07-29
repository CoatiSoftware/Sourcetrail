"use strict";

KIT.Interpreter = {
	visibility : KIT.Visibility.None,

	fileName : null,

	createProgramModel : function( data ) {
		var program = new KIT.Model.Program();

		for ( var i = 0; i < data.length; i++ ) {
			this.parseNode( data[i], program, program );
		}

		return program;
	},

	parseNode : function( node, program, namespace ) {
		if ( !this[node.type] ) {
			console.error( 'Unknown node type', node.type, node );
			return;
		}

		this[node.type]( node, program, namespace );
	},

	file : function( node, program, namespace ) {
		this.fileName = node.name.split( '.' )[0];
		this.parseContent( node.content, program, namespace );
	},

	parseContent : function( content, program, namespace ) {
		if ( !content ) {
			return;
		}

		for ( var i = 0; i < content.length; i++ ) {
			this.parseNode( content[i], program, namespace );
		}
	},

	class : function( node, program, namespace ) {
		var classModel = namespace.createClass( node.name, this.createLocation( node.line ), this.visibility );

		if ( node.base ) {
			program.createInheritance( program.getClass( node.base ), classModel, this.createLocation( node.line ) );
		}

		if ( node.content ) {
			var visibility = this.visibility;

			if ( node.content.public ) {
				this.visibility = KIT.Visibility.Public;
				this.parseContent( node.content.public, program, classModel );
			}

			if ( node.content.private ) {
				this.visibility = KIT.Visibility.Private;
				this.parseContent( node.content.private, program, classModel );
			}

			this.visibility = visibility;
		}
	},

	member : function( node, program, namespace ) {
		var member = namespace.createMember( node.name, this.createLocation( node.line ), this.visibility, this.getAbstraction( node ) );
		if ( node.class ) {
			member.typeName = node.class;
			program.createInstantiation( member, program.getClass( node.class ), this.createLocation( node.line ) );
		}
	},

	method : function( node, program, namespace ) {
		var methodModel = namespace.createMethod( node.name, this.createLocation( node.line ), this.visibility, this.getAbstraction( node ) );
		this.addParams( methodModel, node, program );
		this.parseContent( node.content, program, methodModel );
	},

	getAbstraction : function( node ) {
		switch ( node.abstraction ) {
			case 'static' : return KIT.Abstraction.Static;
			case 'virtual' : return KIT.Abstraction.Virtual;
			case 'pure_virtual' : return KIT.Abstraction.PureVirtual;
			default : return KIT.Abstraction.None;
		}
	},

	use : function( node, program, namespace ) {
		var location = this.createLocation( node.line );
		if ( node.class && node.member ) {
			program.createUsage( namespace, program.getClass( node.class ).getMember( node.member ), location );
		} else if ( node.member ) {
			program.createUsage( namespace, namespace.parent.getMember( node.member ), location );
		} else if ( node.variable ) {
			program.createUsage( namespace, program.getVariable( node.variable ), location );
		} else {
			program.createUsage( namespace, program.getClass( node.class ), location );
		}
	},

	call : function( node, program, namespace ) {
		var location = this.createLocation( node.line );
		if ( node.class ) {
			program.createCall( namespace, program.getClass( node.class ).getMethod( node.method ), location );
		} else if ( node.function ) {
			program.createCall( namespace, program.getFunction( node.function ), location );
		} else {
			program.createCall( namespace, namespace.parent.getMethod( node.method ), location );
		}
	},

	variable : function( node, program, namespace ) {
		var variable = namespace.createVariable( node.name, this.createLocation( node.line ) );
		if ( node.class ) {
			variable.typeName = node.class;
			program.createInstantiation( variable, program.getClass( node.class ), this.createLocation( node.line ) );
		}
	},

	function : function( node, program, namespace ) {
		var functionModel = program.createFunction( node.name, this.createLocation( node.line ) );
		this.addParams( functionModel, node, program );
		this.parseContent( node.content, program, functionModel );
	},

	createLocation : function( lineNumber ) {
		return new KIT.Location( this.fileName, lineNumber );
	},

	addParams : function( functionModel, node, program ) {
		if ( node.params ) {
			for ( var i = 0; i < node.params.length; i++ ) {
				program.createUsage( functionModel, program.getClass( node.params[i] ), this.createLocation( node.line ) );
			}
			functionModel.params = node.params;
		}

		if ( node.return ) {
			program.createUsage( functionModel, program.getClass( node.return ), this.createLocation( node.line ) );
			functionModel.returnValue = node.return;
		}
	}
};
