KIT.View.AnimationLayout = function( program, center ) {
	this.program_ = program;
	this.center_ = center;

	this.nodes_ = [];
	this.edges_ = [];

	this.oldNodes_ = [];
};

KIT.View.AnimationLayout.Node = function( nodeView, layout ) {
	this.view = nodeView.lastParent;
	this.layout_ = layout;

	this.active = !!nodeView.activeChildren.length;
	this.wasVisible = false;
	this.isCenter = false;
	this.wasCenter = false;

	this.position = null;

	this.start = function() {
		if ( this.position ) {
			this.view.update();
			this.view.position = this.position;
		}
		this.position = null;
	}
};

KIT.View.AnimationLayout.Edge = function( edgeView, from, to, layout ) {
	this.view = edgeView;
	this.from = from;
	this.to = to;
	this.layout_ = layout;

	this.start = function() {
		this.view.add();
	}
};

KIT.View.AnimationLayout.prototype = {

	start : function() {
		var center = this.center_.clone();
		var vector = new Vector( -1, 0 );
		var slots = [];

		var cbCount = 0;
		var cb = bind( this, function() {
			cbCount--;
			if ( !cbCount ) {
				this.nodes_.forEach( function( n ) { n.start(); });
				this.edges_.forEach( function( e ) { e.start(); });
			}
		});

		var hasActive = false;
		this.nodes_.forEach( function( n ) { if ( n.active ) hasActive = true; } );
		if ( ( !hasActive && this.nodes_.length > 2 ) || this.nodes_.length === 1 ) {
			this.nodes_[ Math.floor( this.nodes_.length / 2 ) ].active = true;
		}

		if ( this.nodes_.length === 2 ) {
			vector.mulSelf( center.x );
			center.x += vector.norm() / 2;

			slots.push( center.add( vector ) );
			slots.push( center );
		} else {
			var angle = TAU / ( this.nodes_.length - 1 );
			for ( var i = 0; i < this.nodes_.length - 1; i++ ) {
				var vec = vector.clone();
				vec.x *= center.x * 0.7;
				vec.y *= center.y * 0.7;
				slots.push( vec.addSelf( center ) );
				vector.rotateSelf( angle );
			}
		}

		for ( var i = 0; i < this.nodes_.length; i++ ) {
			var node = this.nodes_[i];
			if ( !node.active && node.wasVisible && !node.wasCenter ) {
				var l = Infinity;
				var idx = -1;
				for ( var j = 0; j < slots.length; j++ ) {
					vector.copy( slots[j] ).subSelf( node.view.position );
					if ( vector.normSquared() < l ) {
						l = vector.normSquared();
						idx = j;
					}
				}
				node.position = slots.splice( idx, 1 )[0];
			}
		}

		for ( var i = 0; i < this.nodes_.length; i++ ) {
			var node = this.nodes_[i];

			if ( node.active && this.nodes_.length !== 2 ) {
				node.position = center;
				node.isCenter = true;
			} else if ( !node.position ) {
				node.position = slots.shift();
			}

			if ( node.wasVisible ) {
				node.view.update();
				if ( node.view.animateTo( node.position, cb ) ) {
					cbCount++;
				}
				node.position = null;
			}
		}

		if ( !cbCount ) {
			cbCount++;
			cb();
		}
	},

	stop : function() {
		this.oldNodes_ = this.nodes_;
		this.nodes_ = [];
		this.edges_ = [];
	},

	addNode : function( nodeView ) {
		var node = new KIT.View.AnimationLayout.Node( nodeView, this );
		if ( this.oldNodes_.length ) {
			var oldNodes = this.oldNodes_.filter( function( n ) { return n.view === nodeView; } );
			if ( oldNodes.length ) {
				node.wasVisible = true;
				node.wasCenter = oldNodes[0].isCenter;
			}
		}
		this.nodes_.push( node );
	},

	addEdge : function( edgeView ) {
		var from = this.nodes_.filter( function( n ) { return n.view === edgeView.from.lastParent; })[0];
		var to = this.nodes_.filter( function( n ) { return n.view === edgeView.to.lastParent; })[0];

		if ( !from || !to ) {
			console.error( edgeView.model.name, from, to, 'Edge is not connected to nodes' );
		}

		this.edges_.push( new KIT.View.AnimationLayout.Edge( edgeView, from, to, this ) );
	}

};
