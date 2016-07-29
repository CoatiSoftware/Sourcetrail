KIT.Location = function( fileName, lineNumber ) {
	this.fileName = fileName;
	this.lineNumber = lineNumber
};

KIT.Location.prototype = {
	equals : function( location ) {
		return this.fileName === location.fileName && this.lineNumber === location.lineNumber;
	},

	highlight : function() {
		$( '#' + this.fileName + ' .gutter .number' + this.lineNumber ).addClass( 'highlighted' );
	}
};
