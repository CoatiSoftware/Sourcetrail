SHIFT.Program = function() {
	SHIFT.Container.call( this );
}

SHIFT.Program.prototype = {
	__proto__ : SHIFT.Container.prototype,

	type : SHIFT.Types.Program,

	SetPaper : function( paper ) {
		this.paper = paper;
	},

	GetClass : function( name ) {
		var names = name.split( '::' );
		var obj;
		if ( names.length === 2 ) {
			var classModel = this.FindBy( this.objects, 'name', names[0] );
			if ( !classModel ) {
				classModel = this.Get( SHIFT.Class, names[0] );
			}
			obj = this.FindBy( classModel.objects, 'name', names[1] );
			if ( obj ) {
				return obj;
			} else {
				return classModel.Get( SHIFT.Class, names[1] );
			}
		}

		obj = this.FindBy( this.objects, 'name', name );
		if ( !obj ) {
			var classes = this.FindAllBy( this.objects, 'type', SHIFT.Types.Class );
			for ( var i = 0; i < classes.length; i++ ) {
				obj = this.FindBy( classes[i].objects, 'name', name );
				if ( obj ) {
					return obj;
				}
			}
			return this.Get( SHIFT.Class, name );
		}
		return obj;
	},

	GetVariable : function( name ) {
		return this.Get( SHIFT.Variable, name );
	},

	GetFunction : function( name ) {
		return this.Get( SHIFT.Function, name );
	},

	CreateClass : function( name, location ) {
		return this.Create( SHIFT.Class, name, location );
	},

	CreateVariable : function( name, location ) {
		return this.Create( SHIFT.Variable, name, location );
	},

	CreateFunction : function( name, location ) {
		return this.Create( SHIFT.Function, name, location );
	},

	AddCall : function( from, to, location ) {
		if ( from !== to ) {
			return this.Connect( SHIFT.Call, from, to, location );
		}
		return null;
	},

	AddInheritance : function( from, to, location ) {
		return this.Connect( SHIFT.Inheritance, from, to, location );
	},

	AddInstantiation : function( from, to, location ) {
		return this.Connect( SHIFT.Instantiation, from, to, location );
	},

	AddUse : function( from, to, location ) {
		return this.Connect( SHIFT.Use, from, to, location );
	},

	AddAssociation : function( from, to ) {
		var typename = SHIFT.Association.prototype.type.name;
		var name = typename + ':' + from.Name() + '->' + to.Name();
		var obj = this.FindBy( this.connections, 'name', name );
		if ( !obj ) {
			obj = this.FindBy( this.connections, 'name', typename + ':' + to.Name() + '->' + from.Name() );
		}
		if ( !obj ) {
			obj = new SHIFT.Association( name, from, to );
			this.connections.push( obj );
		}
		return obj;
	},

	ShowInterface : function() {
		var paper = this.paper;
		paper.clear();

		for ( var i = 0; i < this.objects.length; i++ ) {
			this.objects[i].Show(
				paper,
				randInt( 50, width - 50 ),
				randInt( 50, height - 50 )
			);
		}

		for ( var i = 0; i < this.connections.length; i++ ) {
			if ( this.connections[i].type !== SHIFT.Types.Association ) {
				this.connections[i].Show( paper );
			}
		}

		this.ShowMenu( paper );
	},

	ShowMenu : function( paper ) {
		var setTypesVisible = function( types ) {
			return function( checked ) {
				for ( var i = 0; i < types.length; i++ ) {
					SHIFT.Types[types[i]].visible = checked;
				}
				program.Update();
				set.toFront();
			}
		}

		paper.setStart();

		var rect = paper.rect( -10, -10, 710, 170, 5 ).box();
		var menu = paper.text( 340, 147, 'legend & filters' ).code( 20 );

		// class
		paper.checkbox( 20, 25, 16, true ).click( setTypesVisible( ['Class'] ) );
		paper.rect( 38, 10, 260, 30, 5 ).box();
		text = paper.text( 48, 27, 'type & class & namespace' ).code( 18 );
		text.transform( 'T' + text.getBBox().width / 2 + ' 0' );

		// variable
		paper.checkbox( 20, 65, 16, true ).click( setTypesVisible( ['Variable', 'Member'] ) );
		paper.rect( 40, 53, 150, 22 ).box();
		text = paper.text( 48, 66, 'variable & member' ).code( 14 );
		text.transform( 'T' + text.getBBox().width / 2 + ' 0' );

		// function
		paper.checkbox( 20, 105, 16, true ).click( setTypesVisible( ['Function', 'Method'] ) );
		paper.ellipse( 117, 105, 80, 15 ).box();
		text = paper.text( 48, 107, 'function & method' ).code( 14 );
		text.transform( 'T' + text.getBBox().width / 2 + ' 0' );

		// use
		paper.checkbox( 270, 65, 16, true ).click( setTypesVisible( ['Use'] ) );
		paper.path( 'M 340 62 l 50 8' ).line( SHIFT.Colors.light ).thin();
		text = paper.text( 298, 65, 'use' ).code( 16 );
		text.transform( 'T' + text.getBBox().width / 2 + ' 0' );

		// call
		paper.checkbox( 270, 105, 16, true ).click( setTypesVisible( ['Call'] ) );
		var call = paper.wave( paper.rect( 340, 95, 5, 5 ).stroke( 'white' ), paper.rect( 410, 105, 5, 5 ).stroke( 'white' ) ).line( SHIFT.Colors.light );
		var end = call.points[1];
		paper.arrow( end.x, end.y, end.i ).line( SHIFT.Colors.light );
		text = paper.text( 298, 105, 'call' ).code( 16 );
		text.transform( 'T' + text.getBBox().width / 2 + ' 0' );

		// inheritance
		paper.checkbox( 470, 25, 16, true ).click( setTypesVisible( ['Inheritance'] ) );
		paper.arrow2( paper.rect( 660, 15, 5, 5 ).stroke( 'white' ), paper.rect( 600, 25, 5, 5 ).stroke( 'white' ) ).line( SHIFT.Colors.light );
		text = paper.text( 498, 25, 'inheritance' ).code( 16 );
		text.transform( 'T' + text.getBBox().width / 2 + ' 0' );

		// instantiation
		paper.checkbox( 470, 65, 16, true ).click( setTypesVisible( ['Instantiation'] ) );
		paper.arrow2( paper.rect( 680, 55, 5, 5 ).stroke( 'white' ), paper.rect( 620, 65, 5, 5 ).stroke( 'white' ) ).line( SHIFT.Colors.light ).dash();
		text = paper.text( 498, 65, 'instantiation' ).code( 16 );
		text.transform( 'T' + text.getBBox().width / 2 + ' 0' );

		// association
		paper.checkbox( 470, 105, 16, true ).click( setTypesVisible( ['Association'] ) );
		paper.path( 'M 610 102 l 50 8' ).line( 'lightblue', 4 );
		text = paper.text( 498, 105, 'association' ).code( 16 );
		text.transform( 'T' + text.getBBox().width / 2 + ' 0' );

		var set = paper.setFinish();
		set.toggled = false;

		function handler() {
			set.toggled = !set.toggled;
			var y = rect.getBBox().height - 40;
			set.transform( '...t 0 ' + ( set.toggled ? -y : y ) ).toFront();
		}

		rect.click( handler );
		menu.click( handler );
		handler();
	},

	Update : function() {
		var paper = this.paper;

		for ( var i = 0; i < this.objects.length; i++ ) {
			this.objects[i].Update( paper );
		}

		for ( var i = 0; i < this.connections.length; i++ ) {
			if ( this.connections[i].type !== SHIFT.Types.Association ) {
				this.connections[i].Update( paper );
			}
		}
	},

	Name : function() {
		return '';
	},

	HoverInHandler : function( obj ) {
		obj.Hover();
		var connections = this.GetConnections( obj );

		for ( var i = 0; i < connections.length; i++ ) {
			connections[i].Hover( false );
		}
	},

	HoverOutHandler : function( obj ) {
		obj.Hovered();
		var connections = this.GetConnections( obj );

		for ( var i = 0; i < connections.length; i++ ) {
			connections[i].Hovered();
		}
	},

	GetConnections : function( obj ) {
		var connections = this.FindAllBy( this.connections, 'from', obj );
		return connections.concat( this.FindAllBy( this.connections, 'to', obj ) );
	}
}
