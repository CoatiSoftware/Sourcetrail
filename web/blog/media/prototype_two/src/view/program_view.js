KIT.View.Program = function( model ) {
	KIT.View.Class.call( this, model, this, null );

	this.needsUpdate_ = true;
	this.needsEdgeUpdate_ = true;

	this.edgeViews_ = [];
	this.aggregationViews_ = [];

	this.activeViews_ = [];
	this.activeViewsHistory_ = [];

	this.highlightedView_ = null;
	this.showConnections = true;

	this.layout_ = null;

	for ( var i = 0; i < model.edges.length; i++ ) {
		var EdgeViewType;

		switch ( model.edges[i].type ) {
			case KIT.Types.Usage : EdgeViewType = KIT.View.Usage; break;
			case KIT.Types.Call : EdgeViewType = KIT.View.Call; break;
			case KIT.Types.Inheritance : EdgeViewType = KIT.View.Inheritance; break;
			case KIT.Types.Instantiation : EdgeViewType = KIT.View.Instantiation; break;

			default: console.error( "Unknown edge type!");
		}

		var view = new EdgeViewType( model.edges[i], this, this );
		this.edgeViews_.push( view );
	}
}

KIT.View.Program.prototype = {
	__proto__ : KIT.View.Class.prototype,

	get edgeViews() {
		return this.edgeViews_;
	},

	get aggregationViews() {
		return this.aggregationViews_;
	},

	set activeView( view ) {
		if ( view instanceof KIT.View.Edge ) {
			if ( this.highlightedView_ ) {
				this.highlightedView_.highlighted = false;
				this.highlightedView_.setNeedsUpdate();
			}

			this.activeViews_.forEach( function(n) {
				n.highlighted = false;
				n.setNeedsUpdate();
			})

			this.highlightedView_ = view;
			view.highlighted = true;
			view.showLocation();
			this.update();

			$('input.search').val( view.model.name );
		} else {
			this.activeViews = [ view ];
		}
	},

	set activeViews( views ) {
		var visibleViews = [];
		for ( var i = 0; i < this.nodeViews_.length; i++ ) {
			if ( this.nodeViews_[i].visible ) {
				visibleViews.push( this.nodeViews_[i] );
			}
		}

		if ( this.activeViews_.length ) {
			if ( views.length && this.activeViews_.concat( views ).unique().length <= this.activeViews_.length ) {
				if ( this.highlightedView_ ) {
					this.highlightedView_.highlighted = false;
					this.highlightedView_.setNeedsUpdate();
				}
				this.activeViews_.forEach( function( v ) {
					v.highlighted = true;
					v.showLocation();
				});
				if ( this.activeViews_.length === 1 ) {
					$('input.search').val( this.activeViews_[0].model.name );
				} else {
					$('input.search').val( this.activeViews_.length + ' active elements' );
				}
				return;
			}

			this.activeViews_.forEach( function( v ) {
				v.deactivate();
			});

			this.activeViewsHistory_.push( this.activeViews_ );
		}

		this.activeViews_ = views;

		KIT.FileViewer.resetLast();

		this.showConnections = true;

		this.activeViews_.forEach( function( v ) {
			v.activate();
			v.addLocation();
		});

		if ( !views.noScroll ) {
			KIT.FileViewer.scrollToLine();
		}

		setTimeout( bind( this, function() {
			if ( this.activeViewsHistory_.length ) {
				$( '#back' ).removeClass( 'inactive' );
			} else {
				$( '#back' ).addClass( 'inactive' );
			}

			if ( this.activeViews_.length === 1 ) {
				$('input.search').val( this.activeViews_[0].model.name );
			} else {
				$('input.search').val( this.activeViews_.length + ' active elements' );
			}
		}), 10 );

		this.update();

		for ( var i = 0; i < visibleViews.length; i++ ) {
			if ( !visibleViews[i].visible ) {
				visibleViews[i].close();
			}
		}

		this.highlightedView_ = null;
	},

	initView : function( paper, data ) {
		this.init( paper );
		this.paper_ = paper;

		var center = new Vector( paper.width / 2, paper.height / 2 );
		this.layout_ = new KIT.View.AnimationLayout( this, center );

		KIT.FileViewer.init( this );
		setTimeout( bind( this, function() {
			KIT.FileViewer.showNavigation( data );
			this.model_.highlightLocation();
		}), 5 );

		$('input.search').typeahead( this.model.searchData() );

		var that = this;
		$('input.search').on('typeahead:selected', function( object, datum ) {
			that.activeView = that.getNodeViewForModel( datum.model );
			$( this ).blur();
		});

		// $('input.search').on('typeahead:autocompleted', function( object, datum ) {
		// 	console.log('auto')
		// });

		$('input.search').on('focus', function() {
			setTimeout( bind( this, function() {
				this.setSelectionRange( 0, 999 );
			}), 10 );
		});

		$('#back').click( bind( this, this.back ) );
		$('#all').click( bind( this, this.all ) );

	},

	init : function( paper ) {
		for ( var i = 0; i < this.nodeViews_.length; i++ ) {
			this.nodeViews_[i].init( paper );
		}

		for ( var i = 0; i < this.edgeViews_.length; i++ ) {
			var edgeView = this.edgeViews_[i];
			var nodeViewFrom = this.getNodeViewForModel( edgeView.model.from );
			var nodeViewTo = this.getNodeViewForModel( edgeView.model.to );
			this.edgeViews_[i].init( paper, nodeViewFrom, nodeViewTo );
		}
	},

	setNeedsEdgeUpdate : function() {
		this.needsEdgeUpdate_ = true;
	},

	setNeedsUpdate : function() {
		this.needsUpdate_ = true;
	},

	update : function() {
		if ( this.needsUpdate_ ) {
			this.layout_.stop();

			this.removeEdgeAndAggregationViews_();
			this.createAggregationViews_();
			this.updateNodeViews_();
			this.addEdgeAndAggregationViews_();

			this.layout_.start();
		} else if ( this.needsEdgeUpdate_ ) {
			this.updateEdgeAndAggregationViews();
		}

		this.needsUpdate_ = false;
		this.needsEdgeUpdate_ = false;
	},

	removeEdgeAndAggregationViews_ : function() {
		for ( var i = 0; i < this.edgeViews_.length; i++ ) {
			this.edgeViews_[i].remove();
		}

		for ( var i = 0; i < this.aggregationViews_.length; i++ ) {
			this.aggregationViews_[i].remove();
			this.aggregationViews_[i].hide();
		}
		this.aggregationViews_ = [];
	},

	createAggregationViews_ : function() {
		for ( var i = 0; i < this.edgeViews_.length; i++ ) {
			var edgeView = this.edgeViews_[i];
			if ( !edgeView.visible ) {
				var from = edgeView.from.lastParent;
				var to = edgeView.to.lastParent;

				if ( from !== to && this.showConnections &&
					( ( from.active || to.active ) || !this.activeViews_.length ) ) {
					var aggregationView;
					for ( var j = 0; j < this.aggregationViews_.length; j++ ) {
						aggregationView = this.aggregationViews_[j];
						if ( aggregationView.from === from && aggregationView.to === to ||
							aggregationView.to === from && aggregationView.from === to) {
							break;
						}
						aggregationView = null;
					}

					if ( !aggregationView ) {
						aggregationView = new KIT.View.Aggregation( this );
						aggregationView.init( this.paper_, from, to );
						this.aggregationViews_.push( aggregationView );
					}

					aggregationView.show( edgeView );
				}
			}
		}
	},

	updateNodeViews_ : function() {
		for ( var i = 0; i < this.nodeViews_.length; i++ ) {
			var node = this.nodeViews_[i];
			if ( node.visible || !this.activeViews_.length ) {
				this.layout_.addNode( node );
			} else {
				node.remove();
			}
		}
	},

	addEdgeAndAggregationViews_ : function() {
		for ( var i = 0; i < this.edgeViews_.length; i++ ) {
			var edge = this.edgeViews_[i];
			if ( edge.visible ) {
				this.layout_.addEdge( edge );
			}
		}

		for ( var i = 0; i < this.aggregationViews_.length; i++ ) {
			var aggregation = this.aggregationViews_[i];
			this.layout_.addEdge( aggregation );
		}
	},

	updateEdgeAndAggregationViews : function() {
		for ( var i = 0; i < this.edgeViews_.length; i++ ) {
			var edge = this.edgeViews_[i];
			if ( edge.visible ) {
				edge.update();
			}
		}

		for ( var i = 0; i < this.aggregationViews_.length; i++ ) {
			var aggregation = this.aggregationViews_[i];
			aggregation.update();
		}
	},

	setUpdateAggregationViews : function( nodeView ) {
		for ( var i = 0; i < this.aggregationViews_.length; i++ ) {
			var aggregation = this.aggregationViews_[i];
			if ( aggregation.from === nodeView || aggregation.to === nodeView ) {
				aggregation.setNeedsUpdate();
			}
		}
	},

	back : function() {
		if ( this.activeViewsHistory_.length ) {
			this.activeViews = this.activeViewsHistory_.pop();
			this.activeViewsHistory_.pop();
		}
	},

	all : function() {
		this.setNeedsUpdate();
		this.activeViews = [];
	},

	activateLocation : function( location ) {
		var views = [];
		this.addViewsAtLocation( location, views );

		if ( views.length ) {
			views.noScroll = true;
			this.activeViews = views;
			KIT.FileViewer.showLocation( location );
		}
	},

	addViewsAtLocation : function( location, views ) {
		KIT.View.Class.prototype.addViewsAtLocation.call( this, location, views );

		for ( var i = 0; i < this.edgeViews.length; i++ ) {
			this.edgeViews[i].addViewsAtLocation( location, views );
		}
	},

	toggleConnections : function() {
		this.showConnections = !this.showConnections;
		this.setNeedsUpdate();
	}
};
