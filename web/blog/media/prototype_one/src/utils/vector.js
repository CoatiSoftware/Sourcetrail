var Vector = function(x, y) {

  return this.set(
    x || 0, 
    y || 0
  );

};

Vector.prototype = {
  
  set: function(x, y) {
    
    this.x = x;
    this.y = y;
    
    return this;
    
  },
  
  copy: function(vector) {
    
    return this.set(
      vector.x,
      vector.y
    );
    
  },
  
  clone: function() {
    
    return new Vector(
      this.x,
      this.y
    );
    
  },

  add: function(vector) {

    return new Vector(
      this.x + vector.x, 
      this.y + vector.y
    );

  },

  addSelf: function(vector) {

    this.x += vector.x;
    this.y += vector.y;

    return this;

  },
  
  sub: function(vector) {

    return new Vector(
      this.x - vector.x, 
      this.y - vector.y
    );

  },

  subSelf: function(vector) {

    this.x -= vector.x;
    this.y -= vector.y;

    return this;

  },
  
  mul: function(value) {

    return new Vector(
      this.x * value, 
      this.y * value
    );

  },
  
  mulSelf: function(value) {

    this.x *= value;
    this.y *= value;

    return this;

  },
  
  div: function(value) {
    
    if (!value) {
      
      return new Vector();
      
    }

    return new Vector(
      this.x / value, 
      this.y / value
    );

  },
  
  divSelf: function(value) {
    
    if (value) {
      
      this.x /= value;
      this.y /= value;
      
    }
    
    return this;

  },
  
  dot: function(vector) {
    
    return (this.x * vector.x + this.y * vector.y);
    
  },
  
  normSquared: function() {
    
    return (this.x * this.x + this.y * this.y);
    
  },
  
  norm: function() {
    
    return Math.sqrt(this.normSquared());
    
  },
  
  normalize: function() {
    
    return this.div(this.norm());
    
  },
  
  normalizeSelf: function() {
    
    return this.divSelf(this.norm());
    
  },
  
  clamp: function(value) {
    
    if (this.normSquared() > value * value) {
      
      return this.normalize().mul(value);
      
    }
    
    return this;
    
  },
  
  clampSelf: function(value) {
    
    if (this.normSquared() > value * value) {
      
      return this.normalizeSelf().mulSelf(value);
      
    }
    
    return this;
    
  },
  
  angle: function(vector) {
    
    if (vector) {
      
      return Math.acos(this.dot(vector) / this.norm() / vector.norm());
      
    } else {
      
      return Math.atan2(this.y, this.x);
      
    }
    
  },
  
  rotate: function(angle) {
    
    return this.clone().rotateSelf(angle);
    
  },
  
  rotateSelf: function(angle) {
    
    if ( !angle ) {
      
      return this;
      
    }
    
    return this.set(
      Math.cos(angle) * this.x - Math.sin(angle) * this.y,
      Math.sin(angle) * this.x + Math.cos(angle) * this.y
    );
    
  },
  
  string: function() {
    
    return '( ' + this.x + ' | ' + this.y + ' )';
    
  },
  
  log: function() {
    
    console.log( this.string() );
    
  },
  
  getData : function() {
    
    return { x : this.x, y : this.y };
    
  },

  length : function() {
    return Math.sqrt( this.x * this.x + this.y * this.y);
  }

};
