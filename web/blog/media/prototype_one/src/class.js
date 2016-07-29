SHIFT.Class = function( name ) {
	SHIFT.Object.call( this, name );
	SHIFT.Container.call( this );

	this.base = null;

	this.publicOpen = false;
	this.privateOpen = false;

	this.publics = [];
	this.privates = [];

	this.visibility = SHIFT.Visibility.Public;
	this.abstraction = SHIFT.Abstraction.None;

	this.privateObject = new SHIFT.Object( 'Private' );
	this.privateObject.parent = this;
	this.privateObject.active = true;
	this.privateObject.Open = function() {
		this.parent.Open();
	}
}

SHIFT.Class.prototype = {
	__proto__ : extend( new SHIFT.Object, SHIFT.Container.prototype ),

	type : SHIFT.Types.Class,

	GetMember : function( name ) {
		return this.Get( SHIFT.Member, name );
	},

	GetMethod : function( name ) {
		return this.Get( SHIFT.Method, name );
	},

	CreateMember : function( name, location, visibility, abstraction ) {
		return this.Create( SHIFT.Member, name, location, visibility, abstraction );
	},

	CreateMethod : function( name, location, visibility, abstraction ) {
		return this.Create( SHIFT.Method, name, location, visibility, abstraction );
	},

	CreateClass : function( name, location, visibility, abstraction ) {
		var classModel = this.Create( SHIFT.Class, name, location, visibility, abstraction );
		classModel.dragable = false;
		return classModel;
	},

	Create : function( type, name, location, visibility, abstraction ) {
		var obj = SHIFT.Container.prototype.Create.call( this, type, name, location );
		if ( visibility ) {
			obj.visibility = visibility;
			if ( visibility === SHIFT.Visibility.Public ) {
				this.publics.push( obj );
			} else {
				this.privates.push( obj );
			}
		}
		if ( abstraction ) {
			obj.abstraction = abstraction;
		}
		return obj;
	},

	CreateNode : function( paper, x, y ) {
		var contents = [];
		var width = 0;
		var height = 30;
		var bb;

		var privateTop = 0;
		var privateSet = null;

		if ( this.objects.length > this.publics.length + this.privates.length ) {
			this.publics = this.objects;
		}

		var node, dots;
		if ( this.publicOpen ) {
			if ( this.publics.length ) {
				if ( this.privates.length ) {
					var publicText = paper.text( x, y + height + 10, 'public' ).code( 14 );
					contents.push( publicText );
					width = Math.max( publicText.getBBox().width, width );
					height += 20;
				}

				for ( var i = 0; i < this.publics.length; i++ ) {
					node = this.publics[i].Show( paper, x, y + height );
					if ( node ) {
						contents.push( node );
						bb = node.getBBox();
						height += bb.height + 5;
						width = Math.max( bb.width, width );
					}
				}
			}

			if ( this.privates.length ) {
				for ( var i = 0; i < this.privates.length; i++ ) {
					this.privates[i].parent = this.privateObject;
				}

				var privateText = paper.text( x, y + height + 12, 'private' ).code( 14 );
				privateText.click( this.ClickPrivate, this );
				contents.push( privateText );
				width = Math.max( privateText.getBBox().width, width );
				privateTop = height;
				height += 20;

				if ( this.privateOpen ) {
					for ( var i = 0; i < this.privates.length; i++ ) {
						node = this.privates[i].Show( paper, x, y + height );
						if ( node ) {
							contents.push( node );
							bb = node.getBBox();
							height += bb.height + 5;
							width = Math.max( bb.width, width );
						}
					}
				} else {
					dots = paper.text( x, y + height + 2, "..." ).code( 14 );
					dots.click( this.ClickPrivate, this );
					contents.push( dots );
					height += 10;
				}
			}
		} else if ( this.publics.length ) {
			var dots = paper.text( x, y + height + 2, "..." ).code( 14 );
			dots.click( this.ClickPublic, this );
			contents.push( dots );
			height += 10;
		}

		var text = paper.text( x, y + 18, this.name ).code( 18 );
		width = Math.max( text.getBBox().width, width ) + 20;
		x -= width / 2;

		if ( privateTop ) {
			var privateRect = paper.rect( x, y + privateTop, width, height - privateTop, 5 ).box().toBack();
			privateRect.click( this.ClickPrivate, this );
			contents.push( privateRect );

			if ( !this.privateOpen ) {
				this.privateObject.node = paper.set().push( privateRect, privateText, dots );
				privateRect.hover( this.HoverIn, this.HoverOut, this.privateObject, this.privateObject );
				privateText.hover( this.HoverIn, this.HoverOut, this.privateObject, this.privateObject );
				dots.hover( this.HoverIn, this.HoverOut, this.privateObject, this.privateObject );
			}
		}

		var rect = paper.rect( x, y, width, height, 5 ).box().toBack();
		if ( this.abstraction === SHIFT.Abstraction.PureVirtual ) {
			rect.dash();
		}

		var set = paper.set();
		set.push( rect, text );
		set.push.apply( set, contents );
		text.click( this.ClickPublic, this );
		rect.click( this.ClickPublic, this );
		return set;
	},

	ClickPublic : function() {
		if ( this.move.normSquared() || !this.active ) {
			return;
		}

		this.publicOpen = !this.publicOpen;
		this.LastParent().Update( this.node.paper );
	},

	ClickPrivate : function() {
		if ( this.move.normSquared() || !this.active ) {
			return;
		}

		this.privateOpen = !this.privateOpen;
		this.LastParent().Update( this.node.paper );
	},

	Remove : function() {
		SHIFT.Object.prototype.Remove.call( this );
		this.privateObject.Remove();

		for ( var i = 0; i < this.objects.length; i++ ) {
			this.objects[i].Remove();
		}
	},

	Update : function( paper ) {
		this.Remove();

		this.Show( paper, this.pos.x, this.pos.y );
		this.UpdateConnections( paper );
	},

	UpdateConnections : function( paper ) {
		for ( var i = 0; i < this.objects.length; i++ ) {
			this.objects[i].UpdateConnections( paper );
		}

		SHIFT.Connectable.prototype.UpdateConnections.call( this, paper );
	},

	CheckVirtualCall : function( method ) {
		if ( this.base ) {
			var baseMethod = this.FindBy( this.base.objects, 'name', method.name );
			if ( baseMethod && baseMethod.type === SHIFT.Types.Method &&
				( baseMethod.abstraction === SHIFT.Abstraction.Virtual ||
					baseMethod.abstraction === SHIFT.Abstraction.PureVirtual ) ) {
				var callModel = this.parent.AddCall( baseMethod, method, null );
				callModel.abstraction = SHIFT.Abstraction.Virtual;
			}
		}
	},

	Awake : function() {
		this.active = true;
		this.privateObject.active = true;

		for ( var i = 0; i < this.objects.length; i++ ) {
			this.objects[i].active = true;
		}

		this.LastParent().Update( this.node.paper );
	},

	Fade : function() {
		this.active = false;
		this.privateObject.active = false;
		this.publicOpen = false;
		this.privateOpen = false;

		for ( var i = 0; i < this.objects.length; i++ ) {
			this.objects[i].active = false;
		}

		this.LastParent().Update( this.node.paper );
	},

	Open : function() {
		if ( !this.publicOpen ) {
			this.publicOpen = true;
			this.LastParent().Update( this.node.paper );
		} else if ( !this.privateOpen ) {
			this.privateOpen = true;
			this.LastParent().Update( this.node.paper );
		} else {
			console.error( 'class is already open' );
		}
	}
}
