KIT.View.Node = function( model, program, parent ) {
	KIT.View.View.call( this, model, program );
	this.parent_ = parent;

	this.position_ = new Vector;
	this.oldPosition_ = new Vector( randInt( 100, 600 ), randInt( 100, 400 ) );
	this.center_ = new Vector;

	this.edgeViews_ = [];

	this.visibleCount_ = 0;

	this.dragPoint_ = null;
	this.isDragging_ = false;

	this.bb_;
	this.group_ = null;

	this.animating = false;
};

KIT.View.Node.prototype = {
	__proto__ : KIT.View.View.prototype,

	get color() {
		if ( this.highlighted ) {
			return '#48A65B';
		} else if ( this.visibleCount_ && this.model.location ) {
			return 'black';
		} else if ( this.visibleCount_ ) {
			return '#888';
		} else {
			return 'gray';
		}
	},

	get backgroundColor() {
		if ( this.model.location ) {
			return 'white';
		} else {
			return '#EEE';
		}
	},

	get hoverColor() {
		if ( this.highlighted ) {
			return '#48A65B';
		} else {
			return '#777';
		}
	},

	get textColor() {
		if ( this.model.location ) {
			return 'black';
		} else {
			return '#888';
		}
	},

	get parent() {
		return this.parent_;
	},

	get visibleParent() {
		for ( var p = this; p; p = p.parent ) {
			if ( p.visible ) {
				return p;
			}
		}
		return null;
	},

	get activeChildren() {
		if ( this.active ) {
			return [this];
		}
		return [];
	},

	get lastParent() {
		if ( this.parent.model.type !== KIT.Types.Program ) {
			return this.parent.lastParent;
		}
		return this;
	},

	get edges() {
		return this.edgeViews_;
	},

	get invisibleEdges() {
		return this.edgeViews_.filter( function(e) {
			return !e.visible;
		});
	},

	get invisibleEdgeCount() {
		return this.invisibleEdges.length;
	},

	get position() {
		return this.position_;
	},

	get center() {
		return this.center_.set( this.bb.cx, this.bb.cy );
	},

	set position( position ) {
		if ( this.model.type === KIT.Types.Class && position !== this.oldPosition_ ) {
			position.x -= this.bb.width / 2;
			position.y -= this.bb.height / 2;
		}

		var vector = position.sub( this.position_ );
		this.moveBy( vector );
	},

	moveBy : function( vector ) {
		if ( !vector.normSquared() ) {
			return;
		}

		this.position_.addSelf( vector );
		this.updateBB( vector );

		if ( this.group_ ) {
			var translate = this.paper_.canvas.createSVGTransform();
			translate.setTranslate( vector.x, vector.y );
			this.group_.transform.baseVal.appendItem( translate );
		} else {
			this.shape_.translate( vector.x, vector.y );
		}
	},

	animateTo : function( position, cb ) {
		if ( this.model.type === KIT.Types.Class && position !== this.oldPosition_ ) {
			position.x -= this.bb.width / 2;
			position.y -= this.bb.height / 2;
		}

		var vector = position.sub( this.position_ );
		this.position_.copy( position );

		if ( !vector.normSquared() ) {
			return false;
		}

		var that = this;
		var animation = Raphael.animation(
			{ transform: '...t' + vector.x + ',' + vector.y },
			400,
			'linear',
			function() {
				setTimeout( function() {
					that.animating = false;
					cb();
				}, 200);
			}
		).delay( 200 );

		this.animating = true;

		this.updateBB( vector );
		this.shape_.animate( animation );
		return true;
	},

	get bb() {
		return this.bb_;
	},

	updateBB : function( vector ) {
		if ( this.shape ) {
			var bb = this.bb;
			bb.x += vector.x;
			bb.y += vector.y;
			bb.cx = bb.x + bb.width / 2;
			bb.cy = bb.y + bb.height / 2;
			bb.x2 = bb.x + bb.width;
			bb.y2 = bb.y + bb.height;
		}
	},

	set clickable( enable ) {
		if ( !this.shape ) {
			console.error( 'has no shape' );
		}

		if ( enable ) {
			this.shape.click( this.updateAfterCallback( this.click ), this );
			this.shape.dblclick( this.updateAfterCallback( this.dblclick ), this );
		} else {
			this.shape.unclick();
		}
	},

	set hoverable( enable ) {
		if ( !this.shape ) {
			console.error( 'has no shape' );
		}

		if ( enable ) {
			this.shape.hover(
				this.updateAfterCallback( this.hoverIn ),
				this.updateAfterCallback( this.hoverOut ), this, this );
		} else {
			this.shape.unhover();
		}
	},

	set dragable( enable ) {
		if ( !this.shape ) {
			console.error( 'has no shape' );
		}

		if ( enable ) {
			this.shape.drag(
				this.updateAfterCallback( this.dragMove ),
				this.updateAfterCallback( this.dragStart ),
				this.updateAfterCallback( this.dragEnd ), this, this, this );
			this.dragPoint_ = new Vector;
		} else {
			this.shape.undrag();
		}
	},

	addEdgeView : function( edgeView ) {
		this.edgeViews_.push( edgeView );
	},

	update : function() {
		if ( !this.needsUpdate ) {
			return;
		}

		this.remove();
		this.add();

		this.needsUpdate = false;
	},

	updateEdgeViews_ : function() {
		this.program.setUpdateAggregationViews( this );

		for ( var i = 0; i < this.edgeViews_.length; i++ ) {
			this.edgeViews_[i].setNeedsUpdate();
		}
	},

	init : function( paper ) {
		this.paper_ = paper;
	},

	add : function( isClass ) {
		if ( this.shape_ ) {
			console.error( 'has already a shape' );
		}

		this.shape_ = this.createShape( this.paper_ );
		var bb = this.bb_ = this.shape_.getBBox();

		if ( !isClass ) {
			this.group_ = document.createElementNS( "http://www.w3.org/2000/svg", 'g' );
			this.paper_.canvas.appendChild( this.group_ );
			for ( var i = this.shape_.items.length - 1; i >= 0; i-- ) {
				this.group_.appendChild( this.shape_.items[i].node );
			}
			this.updateBB({ x : 0, y : 0 });
		} else {
			this.shape_.translate( bb.width / 2, bb.height / 2 );
			this.updateBB({ x : bb.width / 2, y : bb.height / 2 });
		}

		if ( this.active ) {
			this.onActivate();
		} else {
			this.onDeactivate();
		}

		this.clickable = true;
		this.hoverable = true;
		this.dragable = true;

		this.position = this.oldPosition_;
	},

	remove : function() {
		if ( this.shape_ ) {
			this.shape_.remove();
			this.shape_ = null;

			this.oldPosition_.copy( this.position_ );
			this.position_.set( 0, 0 );
		}
	},

	createShape : function( paper ) {
		var shape = paper.set();
		this.rect = paper.rect( 0, 0, 20, 10 ).fill( 'blue' );
		shape.push( this.rect );
		return shape;
	},

	activate : function() {
		this.active = true;
		this.highlighted = true;

		if ( this.program.showConnections ) {
			this.showEdges();
		}

		this.show();
		this.onActivate();
	},

	deactivate : function() {
		this.active = false;
		this.highlighted = false;

		if ( this.program.showConnections ) {
			this.hideEdges();
		}

		this.hide();
		this.onDeactivate();
	},

	onActivate : function() {
		if ( this.shape ) {
			this.rect.stroke( this.color, 2 );
		}
	},

	onDeactivate : function() {
		if ( this.shape ) {
			this.rect.stroke( this.color, 1 );
		}
	},

	click : function( e ) {
		if ( this.isDragging_ ) {
			return;
		}

		if ( e.cancelBubble ) {
			return;
		}
		e.cancelBubble = true;

		this.program.activeView = this;
		// var bb = this.bb;
		// this.paper_.rect(bb.x,bb.y,bb.width,bb.height).stroke('green');
	},

	dblclick : function( e ) {
		if ( e.cancelBubble ) {
			return;
		}
		e.cancelBubble = true;

		if ( !this.active || this.animating ) {
			return;
		}

		this.deactivate();
		this.program.toggleConnections();
		this.activate();
	},

	showEdges : function() {
		for ( var i = 0; i < this.edgeViews_.length; i++ ) {
			var edge = this.edgeViews_[i];
			if ( this === edge.from ) {
				edge.to.show();
			} else if ( this === edge.to ) {
				edge.from.show();
			}
		}
	},

	hideEdges : function() {
		for ( var i = 0; i < this.edgeViews_.length; i++ ) {
			var edge = this.edgeViews_[i];
			if ( this === edge.from ) {
				edge.to.hide();
			} else if ( this === edge.to ) {
				edge.from.hide();
			}
		}
	},

	show : function() {
		this.visibleCount_++;
		if ( this.parent.model.type !== KIT.Types.Program ) {
			this.parent.show();
		} else {
			this.setNeedsUpdate();
		}
	},

	hide : function() {
		this.visibleCount_--;
		if ( this.parent.model.type !== KIT.Types.Program ) {
			this.parent.hide();
		} else {
			if ( this.visibleCount_ < 0 ) {
				console.error( 'View is not even visible' );
			}
			this.setNeedsUpdate();
		}
	},

	get visible() {
		return this.active || this.visibleCount_ > 0;
	},

	setNeedsUpdate : function() {
		this.program.setNeedsUpdate();
		this.lastParent.needsUpdate = true;
	},

	hoverIn : function( e ) {
		if ( this.shape ) {
			this.rect.stroke( this.hoverColor );
		}
	},

	hoverOut : function( e ) {
		if ( this.shape ) {
			this.rect.stroke( this.color );
		}
	},

	get isDragging() {
		return !!this.dragPoint_.normSquared();
	},

	dragStart : function( x, y, event ) {
		this.dragPoint_.set( x, y );
		this.isDragging_ = false;
	},

	dragMove : function( dx, dy, x, y ) {
		this.isDragging_ = true;
		this.moveBy( new Vector( x, y ).subSelf( this.dragPoint_ ) );
		this.dragPoint_.set( x, y );
		this.updateEdgeViews_();
	},

	dragEnd : function( e ) {
		this.dragPoint_.set( 0, 0 );
	},

	showLocation : function() {
		if ( this.model.location ) {
			KIT.FileViewer.showLocation( this.model.location );
		}
	},

	addLocation : function() {
		if ( this.model.location ) {
			KIT.FileViewer.addLocation( this.model.location );
		}
	},

	addViewsAtLocation : function( location, views ) {
		if ( this.model.location && location.equals( this.model.location ) ) {
			views.push( this );
		}
	},

	close : function() {}
};


// Derived views.

KIT.View.Function = function( model, program, parent ) {
	KIT.View.Node.call( this, model, program, parent );
};

KIT.View.Function.prototype = {
	__proto__ : KIT.View.Node.prototype,

	showReturnValue : false,
	showParams : false,
	mainElement : null,

	createNameShape : function( paper, shape ) {
		this.text = paper.text( 0, 0, this.model.shortName ).code( 14, this.textColor );
		shape.push( this.text );

		if ( this.invisibleEdgeCount ) {
			var bb = this.text.getBBox();
			this.number = paper.text( bb.width / 2 + 10, 0, this.invisibleEdgeCount ).code( 12, this.textColor );
			shape.push( this.number );
		}

		return shape;
	},

	createReturnCircle : function(paper, shape) {

		var shapeBB = shape.getBBox();
		var show = this.showReturnValue;

		var circleRadius = 5,
			returnValueCircle = paper.circle(shapeBB.x + circleRadius, 0, circleRadius).stroke( this.color ),
			fillColor = this.showReturnValue ? 'white' : 'Gainsboro';

		returnValueCircle.click( this.updateAfterCallback( this.clickReturnCircle ), this );

		returnValueCircle.fill( fillColor );
		returnValueCircle.hover(
			function(){ returnValueCircle.fill(!show ? 'white' : 'Gainsboro'); },
			function(){ returnValueCircle.fill(show ? 'white' : 'Gainsboro'); }
		);

		shape.translate(circleRadius + 8.5, 0);

		shape.push(returnValueCircle);

		return shape;
	},

	createParamsCircle : function(paper, shape) {

		var shapeBB = shape.getBBox();
		var show = this.showParams;

		var circleRadius = 5,
			paramsCircle = paper.circle(shapeBB.x + shapeBB.width + circleRadius, 0, circleRadius), 
			fillColor = this.showParams ? 'white' : 'Gainsboro';

		paramsCircle.click( this.updateAfterCallback( this.clickParamsCircle ), this );

		paramsCircle.fill( fillColor );
		paramsCircle.hover(
			function(){ paramsCircle.fill(!show ? 'white' : 'Gainsboro'); },
			function(){ paramsCircle.fill(show ? 'white' : 'Gainsboro'); }
		);

		shape.translate(-8.5, 0);

		shape.push(paramsCircle);

		return shape;
	},

	createRoundedRect : function( paper, shape ) {

		bb = shape.getBBox();
		var roundedRect = paper.rect( bb.x - 8.5, bb.y - 5.5, bb.width + 16, bb.height + 8, 10 );

		this.mainElement = roundedRect;

		roundedRect.fill( this.backgroundColor ).stroke( this.color );

		roundedRect.toBack();

		shape.push( roundedRect );

		return shape;
	},

	createReturnRect : function( paper, shape ) {

		var returnValueText = paper.text( 0, 0, this.model.returnValue ).code( 14, '#000' ),
			returnValueTextBB = returnValueText.getBBox(),
			nameRectBB = shape.getBBox(),
			returnRectWidth = returnValueTextBB.width + 16,
			overlapOffset = 5,
			returnRect = paper.rect( nameRectBB.x - returnValueTextBB.width - 16, nameRectBB.y,
									returnRectWidth + overlapOffset, nameRectBB.height ),
			returnRectBB = returnRect.getBBox();

		returnValueText.click( this.updateAfterCallback( this.clickReturnCircle ), this );
		returnRect.click( this.updateAfterCallback( this.clickReturnCircle ), this );

		// translate returnValue-text into the return rect
		returnValueText.translate( -(returnValueTextBB.x - nameRectBB.x) - returnRectWidth + 8.5, 0);

		// translate everything to move the center
		shape.translate(returnRectBB.width / 2, 0);
		returnValueText.translate(returnRectBB.width / 2, 0);
		returnRect.translate(returnRectBB.width / 2, 0);

		returnRect.fill( 'Gainsboro' );

		shape.push( returnValueText, returnRect );

		return shape;
	},

	createParamRect : function( paper, shape, paramName, offset ) {

		var paramText = paper.text( 0, 0, paramName ).code( 14, '#000' ),
			paramTextBB = paramText.getBBox(),
			outerRectBB = shape.getBBox(),
			paramRect = paper.rect( outerRectBB.x + outerRectBB.width + offset, outerRectBB.y,
									paramTextBB.width + 16, outerRectBB.height ),
			paramRectBB = paramRect.getBBox();

		paramText.click( this.updateAfterCallback( this.clickParamsCircle ), this );
		paramRect.click( this.updateAfterCallback( this.clickParamsCircle ), this );

		// translate text into the new rect
		paramText.translate( Math.abs(paramTextBB.x - (outerRectBB.x + outerRectBB.width)) + 8.5, 0);

		// translate everything to move the center
		shape.translate(-paramRectBB.width / 2, 0);
		paramText.translate(-paramRectBB.width / 2, 0);
		paramRect.translate(-paramRectBB.width / 2, 0);

		paramRect.fill( 'Gainsboro' );

		shape.push( paramText, paramRect );

		return shape;
	},

	createOuterRect : function( paper, shape ) {

		bb = shape.getBBox();
		this.rect = paper.rect( bb.x, bb.y, bb.width, bb.height);

		this.rect.hide();

		shape.push( this.rect );

		return shape;
	},

	createShape : function( paper ) {
		var shape = paper.set();

		shape = this.createNameShape(paper, shape);

		shape = this.createReturnCircle(paper, shape);

		if ( !this.model.params ) {
			// shape = this.createParamsCircle(paper, shape);
		}

		shape = this.createRoundedRect(paper, shape);

		if ( this.showReturnValue ) {
			shape = this.createReturnRect(paper, shape);
		}

		if ( this.showParams && this.model.params ) {
			for ( var i = 0; i < this.model.params.length; i++ ) {
				var offset = i == 0 ? -5 : 2.5;
				shape = this.createParamRect(paper, shape, this.model.params[i], offset);
			}
		}

		shape = this.createOuterRect(paper, shape);

		return shape;
	},

	clickReturnCircle : function( e ) {
		e.cancelBubble = true;

		this.showReturnValue = !this.showReturnValue;
		this.showParams = !this.showParams;

		this.setNeedsUpdate();
	},

	clickParamsCircle : function( e ) {
		e.cancelBubble = true;

		this.showParams = !this.showParams;

		this.setNeedsUpdate();
	},

	onActivate : function() {
		if ( this.mainElement ) {
			this.mainElement.stroke( this.color, 2 );
		}
	},

	onDeactivate : function() {
		if ( this.mainElement ) {
			this.mainElement.stroke( this.color, 1 );
		}
	},

	hoverIn : function( e ) {
		if ( this.shape ) {
			this.mainElement.stroke( this.hoverColor );
		}
	},

	hoverOut : function( e ) {
		if ( this.shape ) {
			this.mainElement.stroke( this.color );
		}
	},
};


KIT.View.Method = function( model, program, parent ) {
	KIT.View.Function.call( this, model, program, parent );
};

KIT.View.Method.prototype = {
	__proto__ : KIT.View.Function.prototype,

	createShape : function( paper ) {
		var shape = KIT.View.Function.prototype.createShape.call( this, paper );

		if ( this.model.abstraction === KIT.Abstraction.PureVirtual ) {
			this.mainElement.dash();
		}

		return shape;
	}
};


KIT.View.Variable = function( model, program, parent ) {
	KIT.View.Node.call( this, model, program, parent );
};

KIT.View.Variable.prototype = {
	__proto__ : KIT.View.Node.prototype,

	createNameShape : function(paper, shape) {

		this.text = paper.text( 0, 0, this.model.shortName ).code( 14, '#000' );
		shape.push( this.text );

		if ( this.invisibleEdgeCount ) {
			var bb = this.text.getBBox();
			this.number = paper.text( bb.width / 2 + 10, 0, this.invisibleEdgeCount ).code( 12, 'black' );
			shape.push( this.number );
		}

		return shape;
	}, 

	createOuterRect_ : function( paper, shape ) {

		var bb = shape.getBBox();
		this.rect = paper.rect( bb.x - 8.5, bb.y - 5.5, bb.width + 16, bb.height + 8 ).stroke( this.color );
		this.rect.fill( 'white' );
		this.rect.toBack();
		shape.push( this.rect );

		return shape;
	},

	createShape : function( paper ) {

		var shape = paper.set();

		shape = this.createNameShape(paper, shape);

		shape = this.createOuterRect_(paper, shape);
		this.mainElement = this.rect;

		return shape;
	},

	onActivate : function() {
		if ( this.mainElement ) {
			this.mainElement.stroke( this.color, 2 );
		}
	},

	onDeactivate : function() {
		if ( this.mainElement ) {
			this.mainElement.stroke( this.color, 1 );
		}
	},

	hoverIn : function( e ) {
		if ( this.shape ) {
			this.mainElement.stroke( this.hoverColor );
		}
	},

	hoverOut : function( e ) {
		if ( this.shape ) {
			this.mainElement.stroke( this.color );
		}
	},
};


KIT.View.Member = function( model, program, parent ) {
	KIT.View.Variable.call( this, model, program, parent );
};

KIT.View.Member.prototype = {
	__proto__ : KIT.View.Variable.prototype, 

	showType : false,
	mainElement : null,

	createTypeCircle : function(paper, shape) {

		var shapeBB = shape.getBBox();
		var show = this.showType;

		var circleRadius = 5, 
			typeToggleCircle = paper.circle(shapeBB.x + circleRadius, 0, circleRadius), 
			fillColor = show ? 'white' : 'Gainsboro';

		typeToggleCircle.click( this.updateAfterCallback( this.clickCircle ), this );

		typeToggleCircle.fill( fillColor );
		typeToggleCircle.hover(
			function(){ typeToggleCircle.fill(!show ? 'white' : 'Gainsboro'); },
			function(){ typeToggleCircle.fill(show ? 'white' : 'Gainsboro'); }
		);

		shape.translate(circleRadius + 8.5, 0);

		shape.push(typeToggleCircle);

		return shape;
	},

	createMemberRect : function( paper, shape ) {

		var bb = shape.getBBox(), 
			memberRect = paper.rect( bb.x - 8.5, bb.y - 5.5, bb.width + 16, bb.height + 8 );

		memberRect.fill( 'white' );
		memberRect.toBack();

		this.mainElement = memberRect;

		shape.push( memberRect );

		return shape;
	},

	createTypeRect : function( paper, shape ) {

		var typeText = paper.text( 0, 0, this.model.typeName ).code( 14, '#000' ), 
			typeTextBB = typeText.getBBox(), 
			memberTextBB = shape.getBBox();

		var typeRect = paper.rect(memberTextBB.x - typeTextBB.width - 16, memberTextBB.y, 
									typeTextBB.width + 16, memberTextBB.height),
			typeRectBB = typeRect.getBBox();

		typeRect.fill( 'Gainsboro' );

		typeText.translate( -(typeTextBB.x - memberTextBB.x) - typeTextBB.width - 8.5, 0);

		// translate everything to move the center
		shape.translate(typeRectBB.width / 2, 0);
		typeText.translate(typeRectBB.width / 2, 0);
		typeRect.translate(typeRectBB.width / 2, 0);

		typeText.click( this.updateAfterCallback( this.clickCircle ), this );
		typeRect.click( this.updateAfterCallback( this.clickCircle ), this );

		shape.push( typeText, typeRect );

		return shape;
	},

	createOuterRect : function( paper, shape ) {

		bb = shape.getBBox();
		this.rect = paper.rect( bb.x, bb.y, bb.width, bb.height);

		this.rect.hide();

		shape.push( this.rect );

		return shape;
	},

	createShape : function( paper ) {

		if ( !this.model.typeName ) {
			return KIT.View.Variable.prototype.createShape.call(this, paper);
		}

		var shape = paper.set();
		shape = KIT.View.Variable.prototype.createNameShape.call(this, paper, shape);
		shape = this.createTypeCircle(paper, shape);
		shape = this.createMemberRect(paper, shape);

		if ( this.showType ) {
			shape = this.createTypeRect(paper, shape);
		}
		shape = this.createOuterRect(paper, shape);

		if ( this.model.abstraction === KIT.Abstraction.PureVirtual ) {
			this.mainElement.dash();
		}

		return shape;
	},

	clickCircle : function( e ) {
		e.cancelBubble = true;

		if ( !this.model.typeName ) {
			return;
		}

		this.showType = !this.showType;

		this.setNeedsUpdate();
	}
};
