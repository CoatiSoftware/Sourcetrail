var SHIFT = {
	Types : {
		Variable : { name : 'variable', visible : true },
		Use : { name : 'use', visible : true },
		Function : { name : 'function', visible : true },
		Call : { name : 'call', visible : true },
		Class : { name : 'class', visible : true },
		Member : { name : 'member', visible : true },
		Method : { name : 'method', visible : true },
		Inheritance : { name : 'inheritance', visible : true },
		Instantiation : { name : 'instantiation', visible : true },
		Association : { name : 'association', visible : true },
		Program : null,
	},

	Visibility : {
		None : 0,
		Private : 1,
		Protected : 2,
		Public : 3
	},

	Abstraction : {
		None : 0,
		Static : 1,
		Virtual : 2,
		PureVirtual : 3
	},

	Colors : {
		dark : '#555',
		light : '#AAA'
	}
};
