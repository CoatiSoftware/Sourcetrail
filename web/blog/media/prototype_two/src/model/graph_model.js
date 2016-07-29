KIT.Model.Graph = function() {
	this.nodes_ = [];
	this.edges_ = [];
};

KIT.Model.Graph.prototype = {

	get nodes() {
		return this.nodes_;
	},

	get edges() {
		return this.edges_;
	},

	findNode : function( name ) {
		return this.findFirstByKeyInArray( 'name_', name, this.nodes_ );
	},

	getNode : function( name ) {
		var node = this.findNode( name );
		if ( !node ) {
			console.error( 'unknown node: ' + name );
		}
		return node;
	},

	getOrCreateNode : function( type, name, location ) {
		var node = this.findNode( name );

		if ( !node && this.baseClass && !location ) {
			node = this.baseClass.findNode( name );
		}

		if ( !node ) {
			node = new type( name );
			this.nodes_.push( node );
			node.parent = this;
		}

		return node;
	},

	createNode : function( type, name, location ) {
		var node = this.getOrCreateNode( type, name, location );
		node.location = location;
		return node;
	},

	findEdge : function( name ) {
		return this.findFirstByKeyInArray( 'name_', name, this.edges_ );
	},

	createEdge : function( type, from, to, location ) {
		if ( from.parent === to ) {
			return null;
		}

		var name = type.prototype.type.name + ':' + from.name + '->' + to.name;
		var edge = this.findFirstByKeyInArray( 'name_', name, this.edges_ );

		if ( !edge ) {
			edge = new type( name, from, to );
			this.edges_.push( edge );
		}

		edge.addLocation( location );

		return edge;
	},

	findFirstByKeyInArray : function( key, value, array ) {
		for ( var i = 0; i < array.length; i++ ) {
			if ( array[i][key] === value ) {
				return array[i];
			}
		}
		return null;
	},

	findAllByKeyInArray : function( key, value, array ) {
		var all = [];
		for ( var i = 0; i < array.length; i++ ) {
			if ( array[i][key] === value ) {
				all.push( array[i] );
			}
		}
		return all;
	},

	addSearchData : function( data ) {
		KIT.Model.Node.prototype.addSearchData.call( this, data );

		for ( var i = 0; i < this.nodes.length; i++ ) {
			this.nodes[i].addSearchData( data );
		}

		return data;
	},

	highlightLocation : function() {
		for ( var i = 0; i < this.nodes.length; i++ ) {
			this.nodes[i].highlightLocation();
		}

		for ( var i = 0; i < this.edges.length; i++ ) {
			this.edges[i].highlightLocation();
		}
	}
};
