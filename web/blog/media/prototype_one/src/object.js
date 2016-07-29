SHIFT.Object = function( name ) {
	SHIFT.Connectable.call( this );

	this.name = name;

	this.parent = null;
	this.node = null;
	this.location = null;

	this.pos = new Vector;
	this.move = new Vector;

	this.active = false;
	this.dragging = false;
}

SHIFT.Object.prototype = {
	__proto__ : SHIFT.Connectable.prototype,

	dragable : true,

	color : SHIFT.Colors.dark,
	highlight : '#EC3636',

	Show : function( paper, x, y ) {
		if ( !this.type.visible ) {
			return null;
		}

		this.node = this.CreateNode( paper, x, y );
		this.pos.set( x, y );

		if ( this.dragable ) {
			this.node.drag( this.DragMove, this.DragStart, this.DragEnd, this, this, this );
		}

		if ( !this.active ) {
			this.node[0].stroke( SHIFT.Colors.light );
			this.node[1].fill( SHIFT.Colors.light );
			if ( this.type === SHIFT.Types.Class && this.publics.length && !this.publicOpen ) {
				this.node[2].fill( SHIFT.Colors.light );
			}
		}

		this.node[0].hover( this.HoverIn, this.HoverOut, this, this );
		this.node[1].hover( this.HoverIn, this.HoverOut, this, this );
		if ( this.type === SHIFT.Types.Class && this.publics.length && !this.publicOpen ) {
			this.node[2].hover( this.HoverIn, this.HoverOut, this, this );
		}

		this.node[0].mousedown( this.MouseDown, this );
		this.node[1].mousedown( this.MouseDown, this );
		if ( this.type === SHIFT.Types.Class && this.publics.length && !this.publicOpen ) {
			this.node[2].mousedown( this.MouseDown, this );
		}

		return this.node;
	},

	CreateNode : function( paper, x, y ) {
		console.error( 'not implemented' );
	},

	DragStart : function( x, y, event ) {
		if ( event.button === 2 ) {
			return false;
		}

		this.node.oldTransform = this.node[0].transform();
		this.move.set( 0, 0 );
		this.dragging = true;
	},

	DragMove : function( dx, dy ) {
		if ( !this.dragging ) {
			return;
		}

		this.move.set( dx, dy );
		this.node.transform( this.node.oldTransform + 't' + dx + ',' + dy );
		this.UpdateConnections( this.node.paper );
	},

	DragEnd : function() {
		if ( !this.dragging ) {
			return;
		}

		this.pos.addSelf( this.move );
		this.dragging = false;
	},

	HoverIn : function() {
		if ( this.active ) {
			this.parent.HoverInHandler( this );
		}
	},

	HoverOut : function() {
		if ( this.active ) {
			this.parent.HoverOutHandler( this );
		}
	},

	HoverInHandler : function( obj ) {
		this.parent.HoverInHandler( obj );
	},

	HoverOutHandler : function( obj ) {
		this.parent.HoverOutHandler( obj );
	},

	Remove : function() {
		if ( this.node ) {
			this.node.remove();
		}
		this.node = null;
	},

	Update : function( paper ) {
		this.Remove();
		this.Show( paper, this.pos.x, this.pos.y );
	},

	Name : function() {
		var parentName = this.parent.Name();
		if ( parentName.length ) {
			return parentName + '::' + this.name;
		}
		return this.name;
	},

	Hover : function() {
		if ( this.node ) {
			this.node[0].stroke( this.highlight );
		}
	},

	Hovered : function() {
		if ( this.node ) {
			this.node[0].stroke( this.color );
		}
	},

	LastParent : function() {
		if ( this.parent.type !== SHIFT.Types.Program ) {
			return this.parent.LastParent();
		}
		return this;
	},

	NextParentWithNode : function() {
		if ( this.node ) {
			return this;
		} else if ( this.parent.NextParentWithNode ) {
			return this.parent.NextParentWithNode();
		}
		return null;
	},

	GetProgram : function() {
		if ( this.parent.type === SHIFT.Types.Program ) {
			return this.parent;
		}
		return this.parent.GetProgram();
	},

	MouseDown : function( event ) {
		if ( event.button === 0 && this.active && this.location ) {
			SHIFT.FileViewer.ShowLocation( this.location );
		} else if ( event.button === 2 ) {
			if ( this.active ) {
				this.Fade();
			} else {
				this.Awake();
			}
		}
	},

	Awake : function() {
		this.active = true;
		this.node[0].stroke( this.color );
		this.node[1].fill( this.color );
		this.UpdateConnections( this.node.paper );
	},

	Fade : function() {
		this.active = false;
		this.node[0].stroke( SHIFT.Colors.light );
		this.node[1].fill( SHIFT.Colors.light );
		this.UpdateConnections( this.node.paper );
	},

	Open : function() {}
}
