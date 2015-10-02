
/*
 Copyright 2012, Kovid Goyal <kovid@kovidgoyal.net>
 Released under the GPLv3 License
*/


(function() {
  var CalibreUtils,
    __slice = [].slice,
    __indexOf = [].indexOf || function(item) { for (var i = 0, l = this.length; i < l; i++) { if (i in this && this[i] === item) return i; } return -1; };

  CalibreUtils = (function() {

    CalibreUtils.name = 'CalibreUtils';

    function CalibreUtils() {
      if (!this instanceof arguments.callee) {
        throw new Error('CalibreUtils constructor called as function');
      }
      this.dom_attr = 'calibre_f3fa75ca98eb4413a4ee413f20f60226';
      this.dom_data = [];
    }

    CalibreUtils.prototype.retrieve = function(node, key, def) {
      var data, idx;
      if (def == null) {
        def = null;
      }
      idx = parseInt(node.getAttribute(this.dom_attr));
      if (isNaN(idx)) {
        return def;
      }
      data = this.dom_data[idx];
      if (!data.hasOwnProperty(key)) {
        return def;
      }
      return data[key];
    };

    CalibreUtils.prototype.store = function(node, key, val) {
      var idx;
      idx = parseInt(node.getAttribute(this.dom_attr));
      if (isNaN(idx)) {
        idx = this.dom_data.length;
        node.setAttribute(this.dom_attr, idx + '');
        this.dom_data.push({});
      }
      return this.dom_data[idx][key] = val;
    };

    CalibreUtils.prototype.log = function() {
      var args, msg, _ref, _ref1;
      args = 1 <= arguments.length ? __slice.call(arguments, 0) : [];
      if (args) {
        msg = args.join(' ');
        if (typeof window !== "undefined" && window !== null ? (_ref = window.console) != null ? _ref.log : void 0 : void 0) {
          return window.console.log(msg);
        } else if (typeof process !== "undefined" && process !== null ? (_ref1 = process.stdout) != null ? _ref1.write : void 0 : void 0) {
          return process.stdout.write(msg + '\n');
        }
      }
    };

    CalibreUtils.prototype.stack_trace = function() {
      var currentFunction, fn, _results;
      currentFunction = arguments.callee.caller;
      _results = [];
      while (currentFunction) {
        fn = currentFunction.toString();
        this.log(fn);
        _results.push(currentFunction = currentFunction.caller);
      }
      return _results;
    };

    CalibreUtils.prototype.window_scroll_pos = function(win) {
      var x, y;
      if (win == null) {
        win = window;
      }
      if (typeof win.pageXOffset === 'number') {
        x = win.pageXOffset;
        y = win.pageYOffset;
      } else {
        if (document.body && (document.body.scrollLeft || document.body.scrollTop)) {
          x = document.body.scrollLeft;
          y = document.body.scrollTop;
        } else if (document.documentElement && (document.documentElement.scrollLeft || document.documentElement.scrollTop)) {
          y = document.documentElement.scrollTop;
          x = document.documentElement.scrollLeft;
        }
      }
      return [x, y];
    };

    CalibreUtils.prototype.viewport_to_document = function(x, y, doc) {
      var frame, rect, win, wx, wy, _ref;
      if (doc == null) {
        doc = typeof window !== "undefined" && window !== null ? window.document : void 0;
      }
      while (doc !== window.document) {
        frame = doc.defaultView.frameElement;
        rect = frame.getBoundingClientRect();
        x += rect.left;
        y += rect.top;
        doc = frame.ownerDocument;
      }
      win = doc.defaultView;
      _ref = this.window_scroll_pos(win), wx = _ref[0], wy = _ref[1];
      x += wx;
      y += wy;
      return [x, y];
    };

    CalibreUtils.prototype.absleft = function(elem) {
      var r;
      r = elem.getBoundingClientRect();
      return this.viewport_to_document(r.left, 0, elem.ownerDocument)[0];
    };

    CalibreUtils.prototype.abstop = function(elem) {
      var r;
      r = elem.getBoundingClientRect();
      return this.viewport_to_document(r.top, 0, elem.ownerDocument)[0];
    };

    CalibreUtils.prototype.word_at_point = function(x, y) {
      var ans, matches, node, offset, range;
      range = document.caretPositionFromPoint ? document.caretPositionFromPoint(x, y) : document.caretRangeFromPoint(x, y);
      if (range === null) {
        return null;
      }
      node = range.startContainer;
      if ((node != null ? node.nodeType : void 0) !== Node.TEXT_NODE) {
        return null;
      }
      offset = range.startOffset;
      range = document.createRange();
      range.selectNodeContents(node);
      try {
        range.setStart(node, offset);
        range.setEnd(node, offset + 1);
      } catch (error) {
        null;

      }
      range.expand('word');
      ans = range.toString().trim();
      range.detach();
      matches = ans.split(/\b/);
      if (matches.length > 0) {
        return matches[0];
      } else {
        return null;
      }
    };

    CalibreUtils.prototype.setup_epub_reading_system = function(name, version, layout, features) {
      return window.navigator.epubReadingSystem = {
        'name': name,
        'version': version,
        'layoutStyle': layout,
        'hasFeature': function(feature, version) {
          var _ref;
          if (version == null) {
            version = 1.0;
          }
          if ((version === null || version === 1.0) && (_ref = feature.toLowerCase(), __indexOf.call(features, _ref) >= 0)) {
            return true;
          }
          return false;
        }
      };
    };

    return CalibreUtils;

  })();

  if (typeof window !== "undefined" && window !== null) {
    window.calibre_utils = new CalibreUtils();
  }

}).call(this);
