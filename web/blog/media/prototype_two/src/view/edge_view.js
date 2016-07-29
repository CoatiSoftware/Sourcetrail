KIT.View.Edge = function( model, program ) {
	KIT.View.View.call( this, model, program );

	this.nodeViewFrom_ = null;
	this.nodeViewTo_ = null;

	this.shapeThick_ = null;
};

KIT.View.Edge.prototype = {
	__proto__ : KIT.View.View.prototype,

	get color() {
		if ( this.highlighted ) {
			return '#48A65B';
		} else {
			return '#444';
		}
	},

	hoverColor : '#48A65B',

	get from() {
		return this.nodeViewFrom_;
	},

	get to() {
		return this.nodeViewTo_;
	},

	get visible() {
		return this.from.visible && this.to.visible && ( this.from.active || this.to.active || this.active );
	},

	set clickable( enable ) {
		if ( !this.shape_ ) {
			console.error( 'has no shape' );
		}

		if ( enable ) {
			this.shapeThick_.click( this.updateAfterCallback( this.click ), this );
			this.shape_.click( this.updateAfterCallback( this.click ), this );
		} else {
			this.shapeThick_.unclick();
			this.shape_.unclick();
		}
	},

	set hoverable( enable ) {
		if ( !this.shape_ ) {
			console.error( 'has no shape' );
		}

		if ( enable ) {
			this.shapeThick_.hover(
				this.updateAfterCallback( this.hoverIn ),
				this.updateAfterCallback( this.hoverOut ), this, this );
			this.shape_.hover(
				this.updateAfterCallback( this.hoverIn ),
				this.updateAfterCallback( this.hoverOut ), this, this );
		} else {
			this.shapeThick_.unhover();
			this.shape_.unhover();
		}
	},

	init : function( paper, nodeViewFrom, nodeViewTo ) {
		this.paper_ = paper;
		this.nodeViewFrom_ = nodeViewFrom;
		this.nodeViewTo_ = nodeViewTo;

		nodeViewFrom.addEdgeView( this );
		nodeViewTo.addEdgeView( this );
	},

	remove : function() {
		if ( this.shape_ ) {
			this.shape_.remove();
			this.shapeThick_.remove();
			this.shape_ = null;
			this.shapeThick_ = null;

			this.needsUpdate = false;
		}
	},

	setNeedsUpdate : function() {
		this.needsUpdate = true;
		this.program.setNeedsEdgeUpdate();
	},

	update : function() {
		if ( this.shape_ && this.needsUpdate ) {
			this.remove();
			this.add();
			this.needsUpdate = false;
		}
	},

	add : function() {
		if ( !this.nodeViewFrom_.shape || !this.nodeViewTo_.shape ) {
			console.error( 'node has no shape' );
			return;
		}

		if ( this.shape_ ) {
			console.error( 'edge has already a shape' );
		}

		this.shape_ = this.createShape( this.paper_, this.nodeViewFrom_, this.nodeViewTo_ );

		if ( !this.shape_ ) {
			return;
		}

		this.shapeThick_ = this.shape_.clone();
		this.shapeThick_.stroke( 'rgba(0,0,0,0)', 10 ).noDash();

		if ( this.model.type !== KIT.Types.Aggregation ) {
			this.shapeThick_.toFront();
			this.shape_.toFront();
		} else {
			this.shapeThick_.toBack();
			this.shape_.toBack();
		}

		if ( this.active ) {
			this.onActivate();
		} else {
			this.onDeactivate();
		}

		this.clickable = true;
		this.hoverable = true;

		this.needsUpdate = false;
	},

	createShape : function( paper, nodeViewFrom, nodeViewTo ) {
		return paper.connection( nodeViewFrom.bb, nodeViewTo.bb, 'center', 'line' ).stroke( this.color, 2 );
	},

	click : function( e ) {
		if ( e.cancelBubble ) {
			return;
		}
		e.cancelBubble = true;

		this.program.activeView = this;
	},

	activate : function() {
		this.active = true;
		this.highlighted = true;
		this.from.show();
		this.to.show();
		this.onActivate();
	},

	deactivate : function() {
		this.active = false;
		this.highlighted = false;
		this.from.hide();
		this.to.hide();
		this.onDeactivate();
	},

	onActivate : function() {
		if ( this.shape ) {
			this.shape.stroke( this.color );
		}
	},

	onDeactivate : function() {
		if ( this.shape ) {
			this.shape.stroke( this.color );
		}
	},

	hoverIn : function( e ) {
		if ( this.shape ) {
			this.shape.stroke( this.hoverColor );
		}
	},

	hoverOut : function( e ) {
		if ( this.shape ) {
			this.shape.stroke( this.color );
		}
	},

	showLocation : function() {
		if ( this.model.locations.length ) {
			KIT.FileViewer.showLocations( this.model.locations );
		}
	},

	addLocation : function() {
		if ( this.model.locations.length ) {
			KIT.FileViewer.addLocations( this.model.locations );
		}
	},

	addViewsAtLocation : function( location, views ) {
		if ( this.model.locations ) {
			for ( var i = 0; i < this.model.locations.length; i++ ) {
				if ( location.equals( this.model.locations[i] ) ) {
					views.push( this );
					return;
				}
			}
		}
	}
};


// Derived views.

KIT.View.Aggregation = function( program ) {
	KIT.View.Edge.call( this, { type : KIT.Types.Aggregation }, program );

	this.edgeViews_ = [];
};

KIT.View.Aggregation.prototype = {
	__proto__ : KIT.View.Edge.prototype,

	color : 'lightgray',
	hoverColor : 'gray',

	get count() {
		return this.edgeViews_.length;
	},

	show : function( edge ) {
		this.edgeViews_.push( edge );
		if ( this.count === 1 ) {
			this.from.show();
			this.to.show();
		}
	},

	hide : function() {
		this.edgeViews_ = [];
		this.from.hide();
		this.to.hide();
	},

	init : function( paper, nodeViewFrom, nodeViewTo ) {
		this.paper_ = paper;
		this.nodeViewFrom_ = nodeViewFrom;
		this.nodeViewTo_ = nodeViewTo;
	},

	createShape : function( paper, nodeViewFrom, nodeViewTo ) {
		var shape = paper.set();
		var line = paper.connection( nodeViewFrom.bb, nodeViewTo.bb, 'center', 'line' )
		line.stroke( this.color, 3 + this.count );

		var a = new Vector().copy( line.start );
		var b = new Vector().copy( line.end );

		var bbA = nodeViewFrom.bb;
		var bbB = nodeViewTo.bb;

		var u = b.sub( a ).normalizeSelf();
		var alpha = u.angle();
		var v = new Vector();
		var w = new Vector();

		v.copy( u ).mulSelf( w.set( bbA.width / 2 * Math.cos( alpha ), bbA.height / 2 * Math.sin( alpha ) ).norm() );
		a.addSelf( v );

		v.copy( u ).mulSelf( w.set( bbB.width / 2 * Math.cos( alpha ), bbB.height / 2 * Math.sin( alpha ) ).norm() * -1 );
		b.addSelf( v );

		v.copy( a ).addSelf( b ).divSelf( 2 );

		var circle = paper.circle( v.x, v.y - 1, 10 ).fill( 'white' ).stroke( 'black', 3 );
		var number = paper.text( v.x, v.y, this.count ).code( 12, 'black' );

		shape.push( line, number, circle );
		return shape;
	},

	click : function( e ) {
		if ( e.cancelBubble ) {
			return;
		}
		e.cancelBubble = true;

		this.program.activeViews = this.edgeViews_;
	}
};


KIT.View.Usage = function( model, program ) {
	KIT.View.Edge.call( this, model, program );
};

KIT.View.Usage.prototype = {
	__proto__ : KIT.View.Edge.prototype,

	// color : '#e26345',
	// hoverColor : '#FF7050',

	createShape : function( paper, nodeViewFrom, nodeViewTo ) {
		var shape = paper.set();
		var line = paper.connection( nodeViewFrom.bb, nodeViewTo.bb, 'middle', 'line' );
		var arrow = paper.arrow( line.end, 6, 3, true ).fill( this.color );
		shape.push( line, arrow );
		shape.stroke( this.color, 1 );
		return shape;
	}
};


KIT.View.Call = function( model, program ) {
	KIT.View.Edge.call( this, model, program );
};

KIT.View.Call.prototype = {
	__proto__ : KIT.View.Edge.prototype,

	// color : '#6295b2',
	// hoverColor : '#7BBADE',

	createShape : function( paper, nodeViewFrom, nodeViewTo ) {
		var shape = paper.set();
		var line = paper.connection( nodeViewFrom.bb, nodeViewTo.bb, 'middle', 'wave' );
		var arrow = paper.arrow( line.end, 10, 5, false );
		shape.push( line, arrow );
		shape.stroke( this.color, 1 );

		if ( this.model.abstraction === KIT.Abstraction.PureVirtual ) {
			line.dash();
		}

		return shape;
	}
};


KIT.View.Inheritance = function( model, program ) {
	KIT.View.Edge.call( this, model, program );
};

KIT.View.Inheritance.prototype = {
	__proto__ : KIT.View.Edge.prototype,

	createShape : function( paper, nodeViewFrom, nodeViewTo ) {
		var shape = paper.set();
		var line = paper.connection( nodeViewTo.bb, nodeViewFrom.bb, 'middle', 'line' );
		var arrow = paper.arrow( line.end, 20, 10, true ).fill( 'white' );
		shape.push( line, arrow );
		shape.stroke( this.color, 2 );
		return shape;
	}
};


KIT.View.Instantiation = function( model, program ) {
	KIT.View.Edge.call( this, model, program );
};

KIT.View.Instantiation.prototype = {
	__proto__ : KIT.View.Edge.prototype,

	createShape : function( paper, nodeViewFrom, nodeViewTo ) {
		return paper.connection( nodeViewFrom.bb, nodeViewTo.bb, 'middle', 'line' ).stroke( this.color, 2 ).dash();
	},
};
