
/*
 Copyright 2012, Kovid Goyal <kovid at kovidgoyal.net>
 Released under the GPLv3 License
*/


(function() {
  var MathJax, log,
    __bind = function(fn, me){ return function(){ return fn.apply(me, arguments); }; };

  log = window.calibre_utils.log;

  MathJax = (function() {

    MathJax.name = 'MathJax';

    function MathJax() {
      this.load_finished = __bind(this.load_finished, this);
      if (!this instanceof arguments.callee) {
        throw new Error('MathJax constructor called as function');
      }
      this.base = null;
      this.math_present = false;
      this.math_loaded = false;
      this.pending_cfi = null;
      this.hub = null;
    }

    MathJax.prototype.load_mathjax = function(user_config) {
      var script;
      if (this.base === null) {
        log('You must specify the path to the MathJax installation before trying to load MathJax');
        return null;
      }
      script = document.createElement('script');
      script.type = 'text/javascript';
      script.src = 'file://' + this.base + '/MathJax.js';
      script.text = user_config + 'MathJax.Hub.signal.Interest(function (message) {if (String(message).match(/error/i)) {console.log(message)}});\nMathJax.Hub.Config({\n    positionToHash: false,\n    showMathMenu: false,\n    extensions: ["tex2jax.js", "asciimath2jax.js", "mml2jax.js"],\n    jax: ["input/TeX","input/MathML","input/AsciiMath","output/SVG"],\n    SVG : { linebreaks : { automatic : true } },\n    TeX: {\n        extensions: ["AMSmath.js","AMSsymbols.js","noErrors.js","noUndefined.js"]\n    }\n        });\nMathJax.Hub.Startup.onload();\nMathJax.Hub.Register.StartupHook("End", window.mathjax.load_finished);\nwindow.mathjax.hub = MathJax.Hub';
      return document.head.appendChild(script);
    };

    MathJax.prototype.load_finished = function() {
      var callback, cfi, _ref;
      log('MathJax load finished!');
      this.math_loaded = true;
      if (this.pending_cfi !== null) {
        _ref = this.pending_cfi, cfi = _ref[0], callback = _ref[1];
        this.pending_cfi = null;
        return window.cfi.scroll_to(cfi, callback);
      }
    };

    MathJax.prototype.check_for_math = function() {
      var c, script, user_config, _i, _len, _ref;
      script = null;
      this.math_present = false;
      this.math_loaded = false;
      this.pending_cfi = null;
      user_config = '';
      _ref = document.getElementsByTagName('script');
      for (_i = 0, _len = _ref.length; _i < _len; _i++) {
        c = _ref[_i];
        if (c.getAttribute('type') === 'text/x-mathjax-config') {
          if (c.text) {
            user_config += c.text;
          }
          script = c;
          c.parentNode.removeChild(c);
        }
      }
      if (script !== null || document.getElementsByTagName('math').length > 0) {
        this.math_present = true;
        return this.load_mathjax(user_config);
      }
    };

    MathJax.prototype.after_resize = function() {
      if (!this.math_present || this.hub === null) {
        return;
      }
      return this.hub.Queue(["Rerender", this.hub]);
    };

    return MathJax;

  })();

  if (typeof window !== "undefined" && window !== null) {
    window.mathjax = new MathJax();
  }

}).call(this);

