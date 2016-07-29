KIT.View.Class = function( model, program, parent ) {
	KIT.View.Node.call( this, model, program, parent );

	this.nodeViews_ = [];

	this.publics_ = [];
	this.privates_ = [];

	for ( var i = 0; i < model.nodes.length; i++ ) {
		var node = model.nodes[i];
		var NodeViewType;

		switch ( node.type ) {
			case KIT.Types.Class : NodeViewType = KIT.View.Class; break;
			case KIT.Types.Variable : NodeViewType = KIT.View.Variable; break;
			case KIT.Types.Member : NodeViewType = KIT.View.Member; break;
			case KIT.Types.Function : NodeViewType = KIT.View.Function; break;
			case KIT.Types.Method : NodeViewType = KIT.View.Method; break;

			default: console.error( "Unknown node type!", node.type );
		}

		var view = new NodeViewType( node, program, this );

		if ( node.visibility === KIT.Visibility.Public ) {
			if ( parent && node.type === KIT.Types.Class ) {
				this.publics_.unshift( view );
			} else {
				this.publics_.push( view );
			}
		} else if ( node.visibility === KIT.Visibility.Private ) {
			if ( parent && node.type === KIT.Types.Class ) {
				this.privates_.unshift( view );
			} else {
				this.privates_.push( view );
			}
		}

		if ( parent && node.type === KIT.Types.Class ) {
			this.nodeViews_.unshift( view );
		} else {
			this.nodeViews_.push( view );
		}
	}

	this.open_ = false;
	this.publicOpen_ = false;
	this.privateOpen_ = false;
}

KIT.View.Class.prototype = {
	__proto__ : KIT.View.Node.prototype,

	get children() {
		return this.nodeViews_;
	},

	get activeChildren() {
		var children = [];
		for ( var i = 0; i < this.nodeViews_.length; i++ ) {
			children = children.concat( this.nodeViews_[i].activeChildren );
		}
		if ( this.active ) {
			children.push( this );
		}
		return children;
	},

	get nodeViews() {
		return this.nodeViews_;
	},

	get invisibleEdgeCount() {
		var sum = 0;
		var nodes = this.nodeViews_;
		if ( nodes.length ) {
			var a = [];
			for ( var i = 0; i < nodes.length; i ++ ) {
				a = a.concat(nodes[i].invisibleEdges);
			}
			sum += a.unique().filter( function(e) {
				return e.from.lastParent !== e.to.lastParent;
			}).length;
		}

		sum += this.edgeViews_.filter( function(e) {
			return !e.visible;
		}).length;

		return sum;
	},

	get openContent() {
		if ( !this.nodeViews.length ) {
			return [];
		}

		var open = this.open_;
		return this.nodeViews_.filter( function( node ) {
			return open || node.visible;
		});
	},

	get openPublics() {
		if ( !this.publics_.length ) {
			return [];
		}

		var open = this.publicOpen_;
		return this.publics_.filter( function( node ) {
			return open || node.visible;
		});
	},

	get openPrivates() {
		if ( !this.privates_.length ) {
			return [];
		}

		var open = this.privateOpen_;
		return this.privates_.filter( function( node ) {
			return open || node.visible;
		});
	},

	toggle : function( e, key ) {
		if ( e.cancelBubble ) {
			return;
		}
		e.cancelBubble = true;

		this[key] = !this[key];

		this.setNeedsUpdate();
	},

	init : function( paper ) {
		for ( var i = 0; i < this.nodeViews_.length; i++ ) {
			this.nodeViews_[i].init( paper );
		}

		KIT.View.Node.prototype.init.call( this, paper );
	},

	remove : function() {
		for ( var i = 0; i < this.nodeViews_.length; i++ ) {
			this.nodeViews_[i].remove();
		}

		KIT.View.Node.prototype.remove.call( this );

		if ( this.visibleCount_ === 0 && this.parent === KIT.Types.Program ) {
			this.open_ = false;
			this.publicOpen_ = false;
			this.privateOpen_ = false;
		}
	},

	getNodeViewForModel : function( model ) {
		for ( var i = 0; i < this.nodeViews_.length; i++ ) {
			var nodeView = this.nodeViews_[i];
			if ( nodeView.model === model ) {
				return nodeView;
			} else if ( nodeView.model.type === KIT.Types.Class ) {
				nodeView = nodeView.getNodeViewForModel( model );
				if ( nodeView ) {
					return nodeView;
				}
			}
		}
		return null;
	},

	getEdgeViewForModel : function( model ) {
		for ( var i = 0; i < this.edgeViews_.length; i++ ) {
			var edgeView = this.edgeView_[i];
			if ( edgeView.model === model ) {
				return edgeView;
			}
		}
		return null;
	},

	updateEdgeViews_ : function() {
		KIT.View.Node.prototype.updateEdgeViews_.call( this );

		for ( var i = 0; i < this.nodeViews_.length; i++ ) {
			this.nodeViews_[i].updateEdgeViews_();
		}
	},

	createShape : function( paper ) {
		var shape = paper.set();
		var box = { width : 0, y : 0, lines : [], cells : 1 };
		var bb, bb2;

		var publics = this.openPublics;
		var privates = this.openPrivates;
		var content = this.openContent;

		this.text = paper.text( 8, 10, this.model.shortName ).code( 18, this.textColor );
		bb = this.text.getBBox();
		this.text.translate( bb.width / 2, bb.height / 2 );
		shape.push( this.text );

		box.y = bb.height + 15;
		box.width = bb.width;

		if ( this.publics_.length || this.privates_.length ) {

			box.cells= Math.max( Math.round( Math.sqrt( publics.length ) ),
								 Math.round( Math.sqrt( privates.length ) ) );

			var text;
			if ( this.publics_.length ) {
				text = paper.text( 35, box.y + 15, '+ public' ).code( 13, '#000' );
				box.width = Math.max( box.width, text.getBBox().width );
				box.lines.push( box.y );
				box.y += 30;

				shape.push( text );

				this.addContent( shape, paper, box, this.publics_.length, publics, 'publicOpen_' );
			}

			if ( this.privates_.length ) {
				text = paper.text( 38, box.y + 15, '- private' ).code( 13, '#000' );
				box.width = Math.max( box.width, text.getBBox().width );
				box.lines.push( box.y );
				box.y += 30;

				shape.push( text );

				this.addContent( shape, paper, box, this.privates_.length, privates, 'privateOpen_' );
			}

		} else if ( this.nodeViews_.length ) {
			box.lines.push( box.y );
			box.y += 10;

			this.addContent( shape, paper, box, this.nodeViews_.length, content, 'open_' );
		}

		box.width += 20;

		if ( !this.active ) {
			this.number = paper.text( 0, 20, this.invisibleEdgeCount ).code( 14, this.textColor );
			bb2 = this.number.getBBox();
			if ( box.width < bb.width + 20 + bb2.width ) {
				this.number.translate( bb.width + bb2.width + 15 );
				box.width += bb2.width + 10;
			} else {
				this.number.translate( box.width - 10 - bb2.width / 2 );
			}
			shape.push( this.number );
		}

		for ( var i = 0; i < box.lines.length; i++ ) {
			var y = box.lines[i] + 0.5;
			shape.push( paper.line( 0, y, box.width, y ).stroke( this.color, 1 ) );
		}

		this.rect = paper.rect( 0, 0, box.width, box.y, 5 )
		this.rect.fill( this.backgroundColor );
		this.rect.toBack();

		if ( this.model.abstraction === KIT.Abstraction.PureVirtual ) {
			this.rect.dash();
		}

		shape.push( this.rect );
		return shape;
	},

	addContent : function( shape, paper, box, length, content, key ) {
		if ( content.length ) {
			var startX = 7, x = startX;
			var gapX = 15, gapY = 10;
			var h = 0;
			var vec = new Vector;

			for ( var i = 0; i < content.length; i++ ) {
				if ( i && i % box.cells === 0 ) {
					box.width = Math.max( box.width, x - gapX );
					x = startX;

					box.y += h + gapY;
					h = 0;
				}

				var node = content[i];
				node.add( true );
				node.dragable = false;

				for ( var j = 0; j < node.shape.items.length; j++ ) {
					shape.push( node.shape.items[j] );
				}

				var bb = node.bb;
				vec.set( x, box.y );
				node.position = vec;

				h = Math.max( h, bb.height );
				x += bb.width + gapX;
			}

			box.width = Math.max( box.width, x - gapX );
			box.y += h + gapY;
		}

		var cb = this.updateAfterCallback( function(e) { this.toggle( e, key ); });
		var rect = paper.rect( 5, box.y - 7, 40, 20 ).fill( this.backgroundColor ).stroke();
		rect.click( cb, this );

		var number = null;
		var point = { x : 20, y : box.y - 2, nx : 0, ny : 1 };
		if ( length > content.length ) {
			number = paper.text( 35, box.y + 3, length - content.length ).code( 13, this.textColor );
			number.click( cb, this );
			shape.push( number );

			point.ny = -1;
			point.y += 8;
		}

		var arrow = paper.arrow( point, 8, 6, true ).stroke( 'gray', 2 ).round();
		arrow.click( cb, this );
		shape.push( arrow );
		shape.push( rect );

		var hoverIn = function() { arrow.stroke('black'); };
		var hoverOut = function() { arrow.stroke('gray'); };
		arrow.hover(hoverIn, hoverOut);
		rect.hover(hoverIn, hoverOut);
		if ( number ) number.hover(hoverIn, hoverOut);

		box.y += 15;
	},

	addViewsAtLocation : function( location, views ) {
		KIT.View.Node.prototype.addViewsAtLocation.call( this, location, views );

		for ( var i = 0; i < this.children.length; i++ ) {
			this.children[i].addViewsAtLocation( location, views );
		}
	},

	updateBB : function( vector ) {
		KIT.View.Node.prototype.updateBB.call( this, vector );

		for ( var i = 0; i < this.children.length; i++ ) {
			this.children[i].updateBB( vector );
		}
	},

	close : function() {
		this.open_ = false;
		this.publicOpen_ = false;
		this.privateOpen_ = false;
	}
};
