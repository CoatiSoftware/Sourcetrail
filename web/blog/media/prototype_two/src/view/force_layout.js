KIT.View.ForceLayout = function( program, center ) {
	this.program_ = program;
	this.center_ = center;

	this.nodes_ = [];
	this.edges_ = [];

	this.time_ = null;
};

KIT.View.ForceLayout.Node = function( nodeView, layout ) {
	this.view = nodeView.lastParent;
	this.active = !!nodeView.activeChildren.length;
	this.layout_ = layout;

	this.move = new Vector();
	this.velocity = new Vector;
	this.force = new Vector;

	this.dt = 0;
};

KIT.View.ForceLayout.Node.prototype = {

	mass : 3,
	damp : 0.5,

	vector : new Vector,
	acceleration : new Vector,

	get position() {
		return this.view.center;
	},

	start : function() {
		this.view.update();
	},

	update : function( dt ) {
		this.dt += dt;

		this.view.updateEdgeViews_();

		this.force.clampSelf( 2000 );

		this.acceleration.copy( this.force ).mulSelf( 1 / this.mass );
		this.force.set( 0, 0 );

		if ( this.active ) {
			this.view.moveBy( this.layout_.center_.sub( this.view.center ).mulSelf( 0.1 ) );
			return;
		} else if ( this.view.isDragging ) {
			return;
		}

		this.velocity.addSelf( this.acceleration.mulSelf( dt ) );

		this.move.set( 0, 0 );
		this.move.addSelf( this.acceleration.mulSelf( dt * this.damp ) );
		this.move.addSelf( this.acceleration.copy( this.velocity ).mulSelf( dt ) );

		this.velocity.mulSelf( 0.97 );

		if ( this.move.normSquared() > 0.001 ) {
			this.view.moveBy( this.move );
		}
	},

	reset : function() {
		this.velocity.set( 0, 0 );
		this.force.set( 0, 0 );
	}

};

KIT.View.ForceLayout.Edge = function( edgeView, from, to, layout ) {
	this.views = [edgeView];
	this.from = from;
	this.to = to;
	this.layout_ = layout;

	this.length = 0;
	this.nominalLength = 200;

	this.dt = 0;
};

KIT.View.ForceLayout.Edge.prototype = {

	strength : 10,
	vector : new Vector,

	start : function() {
		this.views.forEach( function(v) { v.add(); } );
	},

	update : function( dt ) {
		this.dt += dt;

		this.vector.copy( this.from.position ).subSelf( this.to.position )

		this.length = this.vector.norm();
		this.vector.normalizeSelf().mulSelf( this.strength * ( this.length - this.nominalLength ) );

		this.to.force.addSelf( this.vector );
		this.from.force.addSelf( this.vector.mulSelf( -1 ) );
	}
};

KIT.View.ForceLayout.prototype = {

	stop : function() {
		this.time_ = null;
		this.nodes_ = [];
		this.edges_ = [];
	},

	start : function() {
		if ( this.time_ ) {
			return;
		}

		this.time_ = Date.now();

		this.nodes_.forEach( function( n ) { n.start(); });
		this.edges_.forEach( function( e ) { e.start(); });

		requestAnimationFrame( bind( this, this.tick ) );
	},

	tick : function( dt ) {
		if ( this.time_ ) {
			requestAnimationFrame( bind( this, this.tick ) );
		} else {
			return;
		}

		var t = Date.now();
		var dt = ( t - this.time_ ) / 1000;

		this.time_ = t;

		this.update( dt );
		this.program_.update();
	},

	update : function( dt ) {
		this.edges_.forEach( function( e ) { e.update( dt ); });

		var vector = new Vector;
		for ( var i = 0; i < this.nodes_.length; i++ ) {
			var a = this.nodes_[i];
			for ( var j = i + 1; j < this.nodes_.length; j++ ) {
				var b = this.nodes_[j];

				vector.copy( a.view.center ).subSelf( b.view.center );
				if ( vector.normSquared() > 200 * 200 ) {
					continue;
				}

				var push = true;
				// for ( var k = 0; k < this.edges_.length; k++ ) {
				// 	var from = this.edges_[k].from;
				// 	var to = this.edges_[k].to;
				// 	if ( ( from === a && to === b ) || ( from === b && to === a ) ) {
				// 		push = false;
				// 		break;
				// 	}
				// }

				if ( push ) {
					var norm = vector.norm();
					vector.mulSelf( ( 200 - norm ) / norm * 30 );
					a.force.addSelf( vector );
					b.force.addSelf( vector.mulSelf( -1 ) );
				}
			}
		}

		this.nodes_.forEach( function( n ) { n.update( dt ); });
	},

	addNode : function( nodeView ) {
		this.nodes_.push( new KIT.View.ForceLayout.Node( nodeView, this ) );
	},

	addEdge : function( edgeView ) {
		var from = this.nodes_.filter( function( n ) { return n.view === edgeView.from.lastParent; })[0];
		var to = this.nodes_.filter( function( n ) { return n.view === edgeView.to.lastParent; })[0];

		if ( !from || !to ) {
			console.error( edgeView.model.name, from, to, 'Edge is not connected to nodes' );
		}

		var oldEdge = this.edges_.filter( function( e ) { return e.from === from && e.to === to; });
		if ( oldEdge.length ) {
			oldEdge[0].views.push( edgeView );
		} else {
			this.edges_.push( new KIT.View.ForceLayout.Edge( edgeView, from, to, this ) );
		}
	}

};
