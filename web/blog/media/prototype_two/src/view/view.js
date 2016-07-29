KIT.View.View = function( model, program ) {
	this.model_ = model;
	this.program_ = program;

	this.paper_ = null;
	this.shape_ = null;

	this.active = false;
	this.highlighted = false;

	this.needsUpdate = false;
};

KIT.View.View.prototype = {

	get model() {
		return this.model_;
	},

	get program() {
		return this.program_;
	},

	get paper() {
		return this.paper_;
	},

	get shape() {
		return this.shape_;
	},

	add : function() {
		console.error( 'needs to be overriden' );
	},

	remove : function() {
		console.error( 'needs to be overriden' );
	},

	activate : function() {
		console.error( 'needs to be overriden' );
	},

	deactivate : function() {
		console.error( 'needs to be overriden' );
	},

	onActivate : function() {
	},

	onDeactivate : function() {
	},

	updateAfterCallback : function( callback ) {
		var that = this;
		return function() {
			callback.apply( that, arguments );
			that.program.update();
		}
	}
};
