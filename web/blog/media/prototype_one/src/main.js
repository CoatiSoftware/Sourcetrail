var program;
var width;
var height;
var useSample = false;

function main() {

	$( '#wrapper' ).height( $( window ).height() - $( '#header' ).height() - 35 );
	width = $( '#paper' ).width();
	height = $( '#paper' ).height();

	SHIFT.FileViewer.Init();

	// useSample = true;
	if ( useSample || getURLParams().sample ) {
		$( '#sample' ).show();
		sample();
	} else {
		SHIFT.FileViewer.ShowNavigation();
		program = SHIFT.Interpreter.CreateProgram( data );
	}

	program.SetPaper( Raphael( 'paper', width, height ) );
	program.ShowInterface();

	document.getElementById( 'paper' ).addEventListener( 'contextmenu', function( event ) {
		event.preventDefault();
	});
	document.getElementById( 'paper' ).addEventListener( 'selectstart', function( event ) {
		event.preventDefault();
	});
}

function sample() {
	program = new SHIFT.Program();

	// var class1 = program.CreateClass( 'class1', new SHIFT.Location( 'sample', 1 ) );
	// class1.CreateMethod( 'method', new SHIFT.Location( 'sample', 1 ), SHIFT.Visibility.Public );
	// 
	// var class2 = program.CreateClass( 'class2', new SHIFT.Location( 'sample', 1 ) );
	// class2.CreateMethod( 'method', new SHIFT.Location( 'sample', 1 ), SHIFT.Visibility.Public );
	// class2.CreateMember( 'member', new SHIFT.Location( 'sample', 1 ), SHIFT.Visibility.Private );
	// 
	// var class3 = program.CreateClass( 'class3', new SHIFT.Location( 'sample', 1 ) );
	// class3.CreateMethod( 'method', new SHIFT.Location( 'sample', 1 ), SHIFT.Visibility.Public );
	// class3.CreateMember( 'member', new SHIFT.Location( 'sample', 1 ), SHIFT.Visibility.Private );
	// 
	// return;

	// var connection = program.CreateClass( 'Connection' );
	// var use = program.CreateClass( 'Use' );
	// var call = program.CreateClass( 'Call' );
	// var inheritance = program.CreateClass( 'Inheritance' );
	// var instantiation = program.CreateClass( 'Instantiation' );
	// var association = program.CreateClass( 'Association' );
	// 
	// program.AddInheritance( connection, use );
	// program.AddInheritance( connection, call );
	// program.AddInheritance( connection, inheritance );
	// program.AddInheritance( inheritance, instantiation );
	// program.AddInheritance( connection, association );
	// 
	// 
	// var connectable = program.CreateClass( 'Connectable' );
	// var object = program.CreateClass( 'Object' );
	// var variable = program.CreateClass( 'Variable' );
	// var member = program.CreateClass( 'Member' );
	// var func = program.CreateClass( 'Function' );
	// var method = program.CreateClass( 'Method' );
	// var clas = program.CreateClass( 'Class' );
	// var container = program.CreateClass( 'Container' );
	// var prog = program.CreateClass( 'Program' );
	// 
	// program.AddInheritance( connectable, object );
	// program.AddInheritance( object, clas );
	// program.AddInheritance( object, func );
	// program.AddInheritance( object, variable );
	// program.AddInheritance( variable, member );
	// program.AddInheritance( func, method );
	// program.AddInheritance( container, clas );
	// program.AddInheritance( container, prog );
	// 
	// program.AddInstantiation( container.CreateMember( 'objects' ), object );
	// program.AddInstantiation( container.CreateMember( 'connections' ), connection );
	// program.AddInstantiation( connectable.CreateMember( 'connections' ), connection );
	// program.AddInstantiation( connection.CreateMember( 'from' ), connectable );
	// program.AddInstantiation( connection.CreateMember( 'to' ), connectable );
	// 
	// return;

	var playerClass = program.CreateClass( 'Player', new SHIFT.Location( 'sample', 1 ) );
	var playerClassMethodDo = playerClass.CreateMethod( 'Do', new SHIFT.Location( 'sample', 3 ) );
	
	var baseClass = program.CreateClass( 'Base', new SHIFT.Location( 'sample', 6 ) );
	
	var gameClass = program.CreateClass( 'Game', new SHIFT.Location( 'sample', 8 ) );
	var baseGameInheritance = program.AddInheritance( baseClass, gameClass, new SHIFT.Location( 'sample', 8 ) );
	
	var gameClassConstructor = gameClass.CreateMethod( 'Game', new SHIFT.Location( 'sample', 10 ) );
	var gameClassMethodInit = gameClass.CreateMethod( 'Init', new SHIFT.Location( 'sample', 14 ) );
	var gameClassMethodRun = gameClass.CreateMethod( 'Run', new SHIFT.Location( 'sample', 16 ) );
	var gameClassPlayerMember = gameClass.CreateMember( 'player', new SHIFT.Location( 'sample', 20 ) );
	gameClassPlayerMember.typename = 'Player';
	var gameClassPlayerMemberInstantiation = program.AddInstantiation( gameClassPlayerMember, playerClass, new SHIFT.Location( 'sample', 20 ) );
	
	var gameClassMethodInitCall = program.AddCall( gameClassConstructor, gameClassMethodInit, new SHIFT.Location( 'sample', 11 ) );
	var gameClassPlayerMemberUse = program.AddUse( gameClassMethodRun, gameClassPlayerMember );
	var playerClassMethodDoCall = program.AddCall( gameClassMethodRun, playerClassMethodDo, new SHIFT.Location( 'sample', 17 ) );
	
	var gameVariable = program.CreateVariable( 'game', new SHIFT.Location( 'sample', 23 ) );
	var gameVariableGameClassInstantiation = program.AddInstantiation( gameVariable, gameClass, new SHIFT.Location( 'sample', 23 ) );
	gameVariable.typename = 'Game';
	
	var mainFunction = program.CreateFunction( 'main', new SHIFT.Location( 'sample', 25 ) );
	program.AddUse( mainFunction, program.GetClass( 'int' ), new SHIFT.Location( 'sample', 25 ) );
	var gameClassConstructorCall = program.AddCall( mainFunction, gameClassConstructor, new SHIFT.Location( 'sample', 26 ) );
	
	var gameClassMethodRunCall = program.AddCall( mainFunction, gameClassMethodRun, new SHIFT.Location( 'sample', 28 ) );
	
	program.AddUse( mainFunction, gameVariable, new SHIFT.Location( 'sample', 26 ) );
	program.AddUse( mainFunction, gameVariable, new SHIFT.Location( 'sample', 28 ) );
	program.AddUse( mainFunction, gameVariable, new SHIFT.Location( 'sample', 30 ) );
}
