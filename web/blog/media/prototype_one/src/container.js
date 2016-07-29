SHIFT.Container = function() {
	this.objects = [];
	this.connections = [];
}

SHIFT.Container.prototype = {
	Get : function( type, name, ignoreBase ) {
		var obj = this.FindBy( this.objects, 'name', name );

		if ( !ignoreBase && !obj && this.base ) {
			obj = this.FindBy( this.base.objects, 'name', name );
		}

		if ( !obj ) {
			obj = new type( name );
			this.objects.push( obj );
			obj.parent = this;
		}
		return obj;
	},

	Create : function( type, name, location ) {
		var obj = this.Get( type, name, true );
		if ( obj.location ) {
			console.error( 'Location already set!', obj );
		} else {
			obj.location = location;
			obj.active = true;
		}
		return obj;
	},

	Connect : function( type, from, to, location ) {
		var name = type.prototype.type.name + ':' + from.Name() + '->' + to.Name();
		var obj = this.FindBy( this.connections, 'name', name );
		if ( !obj ) {
			obj = new type( name, from, to );
			this.connections.push( obj );
		}
		obj.locations.push( location );
		return obj;
	},

	FindBy : function( array, key, value ) {
		for ( var i = 0; i < array.length; i++ ) {
			if ( array[i][key] === value ) {
				return array[i];
			}
		}
		return null;
	},

	FindAllBy : function( array, key, value ) {
		var all = [];
		for ( var i = 0; i < array.length; i++ ) {
			if ( array[i][key] === value ) {
				all.push( array[i] );
			}
		}
		return all;
	}
}