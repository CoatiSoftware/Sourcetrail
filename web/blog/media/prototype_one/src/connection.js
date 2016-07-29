SHIFT.Connection = function( name, from, to ) {
	this.name = name;

	this.from = from;
	this.to = to;

	this.node = null;
	this.hoverNode = null;

	this.locations = [];
	this.association = null;

	from.AddConnection( this );
	to.AddConnection( this );
}

SHIFT.Connection.prototype = {

	color : SHIFT.Colors.light,
	highlight : '#EC3636',

	Show : function( paper ) {
		if ( !this.type.visible ) {
			return null;
		}

		var from = this.from;
		var to = this.to;

		var fromParent = from.LastParent();
		var toParent = to.LastParent();

		if ( !from.node || !to.node ) {
			this.association = this.CreateAssociation( paper, from.NextParentWithNode(), to.NextParentWithNode() );
		} else {
			var sameClass = fromParent.type === SHIFT.Types.Class && fromParent === toParent;
			this.node = this.CreateNode( paper, from.node, to.node, sameClass );
		}

		if ( this.node ) {
			this.hoverNode = this.node.clone();
			this.hoverNode.line( 'rgba(0,0,0,0)', 10 ).hover( this.Hover, this.Hovered, this, this );
			this.hoverNode.hover( this.Hover, this.Hovered, this, this );
			this.node.hover( this.Hover, this.Hovered, this, this );

			this.node.click( this.Click, this );
			this.hoverNode.click( this.Click, this );

			if ( !from.active || !to.active ) {
				this.node.hide();
				this.hoverNode.hide();
			}
		}

		return this.node;
	},

	CreateNode : function( paper, fromNode, toNode, sameClass ) {
		console.error( 'not implemented' );
	},

	CreateAssociation : function( paper, from, to ) {
		if ( !from || !to || from === to ) {
			return null;
		}

		var association = from.GetProgram().AddAssociation( from, to );
		association.Update( paper );
		association.Increment();
		return association;
	},

	Remove : function() {
		if ( this.node ) {
			this.node.remove();
		}
		if ( this.hoverNode ) {
			this.hoverNode.remove();
		}
		this.node = null;
	},

	Update : function( paper ) {
		this.Remove();

		if ( this.association ) {
			this.association.Decrement();
			this.association = null;
		}

		this.Show( paper );
	},

	Hover : function( direct ) {
		if ( this.node ) {
			if ( !direct ) {
				this.node.show();
			}
			this.node.toFront().stroke( this.highlight );
		}
	},

	Hovered : function() {
		if ( this.node ) {
			if ( !this.from.active || !this.to.active ) {
				this.node.hide();
			}
			this.node.stroke( this.color );
		}
	},

	Click : function() {
		if ( this.locations.length ) {
			SHIFT.FileViewer.ShowLocations( this.locations );
		} else if ( this.type === SHIFT.Types.Association ) {
			this.from.Open();
			this.to.Open();
		}
	}
}

SHIFT.Connectable = function() {
	this.connections = [];
}

SHIFT.Connectable.prototype = {
	AddConnection : function( connection ) {
		if ( connection.from === this ) {
			this.connections.unshift( connection );
		} else if ( connection.to === this ) {
			this.connections.push( connection );
		} else {
			console.error( 'This object is not associated with the connection' );
		}
	},

	UpdateConnections : function( paper ) {
		for ( var i = 0; i < this.connections.length; i++ ) {
			if ( this.connections[i].type !== SHIFT.Types.Association ) {
				this.connections[i].Update( paper );
			}
		}
	}
}