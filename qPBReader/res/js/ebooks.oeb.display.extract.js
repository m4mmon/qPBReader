
/*
 Copyright 2012, Kovid Goyal <kovid at kovidgoyal.net>
 Released under the GPLv3 License
*/


(function() {
  var CalibreExtract, get_containing_block, get_epub_type, get_page_break, get_parents_and_self, hide_children, inline_styles, is_epub_footnote, is_footnote_link, log, merge, trim,
    __indexOf = [].indexOf || function(item) { for (var i = 0, l = this.length; i < l; i++) { if (i in this && this[i] === item) return i; } return -1; },
    __bind = function(fn, me){ return function(){ return fn.apply(me, arguments); }; };

  if (typeof window !== "undefined" && window !== null ? window.calibre_utils : void 0) {
    log = window.calibre_utils.log;
  }

  merge = function(node, cnode) {
    var name, rule, rules, style, val, _i, _len, _results;
    rules = node.ownerDocument.defaultView.getMatchedCSSRules(node, '');
    if (rules) {
      _results = [];
      for (_i = 0, _len = rules.length; _i < _len; _i++) {
        rule = rules[_i];
        style = rule.style;
        _results.push((function() {
          var _j, _len1, _results1;
          _results1 = [];
          for (_j = 0, _len1 = style.length; _j < _len1; _j++) {
            name = style[_j];
            val = style.getPropertyValue(name);
            if (val && !cnode.style.getPropertyValue(name)) {
              _results1.push(cnode.style.setProperty(name, val));
            } else {
              _results1.push(void 0);
            }
          }
          return _results1;
        })());
      }
      return _results;
    }
  };

  inline_styles = function(node) {
    var cnl, cnode, i, nl, _i, _len;
    cnode = node.cloneNode(true);
    merge(node, cnode);
    nl = node.getElementsByTagName('*');
    cnl = cnode.getElementsByTagName('*');
    for (i = _i = 0, _len = nl.length; _i < _len; i = ++_i) {
      node = nl[i];
      merge(node, cnl[i]);
    }
    return cnode;
  };

  get_epub_type = function(node, possible_values) {
    var epub_type, x, _i, _len, _ref, _ref1;
    epub_type = node.getAttributeNS("http://www.idpf.org/2007/ops", 'type') || node.getAttribute('epub:type');
    if (!epub_type) {
      _ref = node.attributes;
      for (_i = 0, _len = _ref.length; _i < _len; _i++) {
        x = _ref[_i];
        if (x.nodeName && (_ref1 = x.nodeValue, __indexOf.call(possible_values, _ref1) >= 0) && x.nodeName.slice(-':type'.length) === ':type') {
          epub_type = x.nodeValue;
          break;
        }
      }
    }
    return epub_type;
  };

  get_containing_block = function(node) {
    var _ref, _ref1;
    while ((_ref = node != null ? (_ref1 = node.tagName) != null ? _ref1.toLowerCase() : void 0 : void 0) !== 'p' && _ref !== 'div' && _ref !== 'li' && _ref !== 'td' && _ref !== 'h1' && _ref !== 'h2' && _ref !== 'h2' && _ref !== 'h3' && _ref !== 'h4' && _ref !== 'h5' && _ref !== 'h6' && _ref !== 'body') {
      node = node.parentNode;
      if (!node) {
        break;
      }
    }
    return node;
  };

  trim = function(str) {
    return str.replace(/^\s\s*/, '').replace(/\s\s*$/, '');
  };

  is_footnote_link = function(node, url, linked_to_anchors) {
    var cb, children, ctext, eid, epub_type, ltext, num, style, x, _ref, _ref1, _ref2;
    if (!url || url.substr(0, 'file://'.length).toLowerCase() !== 'file://') {
      return false;
    }
    epub_type = get_epub_type(node, ['noteref']);
    if (epub_type && epub_type.toLowerCase() === 'noteref') {
      return true;
    }
    if (epub_type && epub_type === 'link') {
      return false;
    }
    _ref = [node, 3], x = _ref[0], num = _ref[1];
    while (x && num > 0) {
      style = window.getComputedStyle(x);
      if ((_ref1 = style.verticalAlign) === 'sub' || _ref1 === 'super') {
        return true;
      }
      x = x.parentNode;
      num -= 1;
    }
    children = (function() {
      var _i, _len, _ref2, _results;
      _ref2 = node.childNodes;
      _results = [];
      for (_i = 0, _len = _ref2.length; _i < _len; _i++) {
        x = _ref2[_i];
        if (x.nodeType === Node.ELEMENT_NODE) {
          _results.push(x);
        }
      }
      return _results;
    })();
    if (children.length === 1) {
      style = window.getComputedStyle(children[0]);
      if ((_ref2 = style.verticalAlign) === 'sub' || _ref2 === 'super') {
        return true;
      }
    }
    eid = node.getAttribute('id') || node.getAttribute('name');
    if (eid && linked_to_anchors.hasOwnProperty(eid)) {
      cb = get_containing_block(node);
      if (!cb || cb.tagName.toLowerCase() === 'body') {
        return false;
      }
      ltext = node.textContent;
      if (!ltext) {
        return false;
      }
      ctext = cb.textContent;
      if (!ctext) {
        return false;
      }
      if (trim(ctext) === trim(ltext)) {
        return false;
      }
      return true;
    }
    return false;
  };

  is_epub_footnote = function(node) {
    var epub_type, pv, _ref;
    pv = ['note', 'footnote', 'rearnote'];
    epub_type = get_epub_type(node, pv);
    if (epub_type && (_ref = epub_type.toLowerCase(), __indexOf.call(pv, _ref) >= 0)) {
      return true;
    }
    return false;
  };

  get_parents_and_self = function(node) {
    var ans;
    ans = [];
    while (node && node !== document.body) {
      ans.push(node);
      node = node.parentNode;
    }
    return ans;
  };

  get_page_break = function(node) {
    var ans, style, x, _i, _len, _ref, _ref1;
    style = getComputedStyle(node);
    ans = {};
    _ref = ['before', 'after'];
    for (_i = 0, _len = _ref.length; _i < _len; _i++) {
      x = _ref[_i];
      ans[x] = (_ref1 = style.getPropertyValue('page-break-'.concat(x))) === 'always' || _ref1 === 'left' || _ref1 === 'right';
    }
    return ans;
  };

  hide_children = function(node) {
    var child, _i, _len, _ref, _results;
    _ref = node.childNodes;
    _results = [];
    for (_i = 0, _len = _ref.length; _i < _len; _i++) {
      child = _ref[_i];
      if (child.nodeType === Node.ELEMENT_NODE) {
        if (child.do_not_hide) {
          _results.push(hide_children(child));
        } else {
          _results.push(child.style.display = 'none');
        }
      } else {
        _results.push(void 0);
      }
    }
    return _results;
  };

  CalibreExtract = (function() {

    CalibreExtract.name = 'CalibreExtract';

    function CalibreExtract() {
      this.extract = __bind(this.extract, this);

      this.mark = __bind(this.mark, this);
      if (!this instanceof arguments.callee) {
        throw new Error('CalibreExtract constructor called as function');
      }
      this.marked_node = null;
    }

    CalibreExtract.prototype.mark = function(node) {
      return this.marked_node = node;
    };

    CalibreExtract.prototype.extract = function(node) {
      var cnode;
      if (node == null) {
        node = null;
      }
      if (node === null) {
        node = this.marked_node;
      }
      cnode = inline_styles(node);
      return cnode.outerHTML;
    };

    CalibreExtract.prototype.is_footnote_link = function(a) {
      return is_footnote_link(a, a.href, py_bridge.value);
    };

    CalibreExtract.prototype.show_footnote = function(target, known_targets) {
      var child, elem, in_note, is_footnote_container, pb, start_elem, _i, _j, _k, _l, _len, _len1, _len2, _len3, _ref, _ref1, _ref2, _ref3;
      if (!target) {
        return;
      }
      start_elem = document.getElementById(target);
      if (!start_elem) {
        return;
      }
      in_note = false;
      is_footnote_container = is_epub_footnote(start_elem);
      _ref = get_parents_and_self(start_elem);
      for (_i = 0, _len = _ref.length; _i < _len; _i++) {
        elem = _ref[_i];
        elem.do_not_hide = true;
      }
      _ref1 = document.body.getElementsByTagName('*');
      for (_j = 0, _len1 = _ref1.length; _j < _len1; _j++) {
        elem = _ref1[_j];
        if (in_note) {
          if (known_targets.hasOwnProperty(elem.getAttribute('id'))) {
            in_note = false;
            continue;
          }
          pb = get_page_break(elem);
          if (pb['before']) {
            in_note = false;
          } else if (pb['after']) {
            in_note = false;
            elem.do_not_hide = true;
            _ref2 = elem.getElementsByTagName('*');
            for (_k = 0, _len2 = _ref2.length; _k < _len2; _k++) {
              child = _ref2[_k];
              child.do_not_hide = true;
            }
          } else {
            elem.do_not_hide = true;
          }
        } else {
          if (elem === start_elem) {
            in_note = !is_footnote_container && !get_page_break(elem)['after'];
            if (!in_note) {
              _ref3 = elem.getElementsByTagName('*');
              for (_l = 0, _len3 = _ref3.length; _l < _len3; _l++) {
                child = _ref3[_l];
                child.do_not_hide = true;
              }
            }
          }
        }
      }
      hide_children(document.body);
      return location.hash = '#' + target;
    };

    return CalibreExtract;

  })();

  if (typeof window !== "undefined" && window !== null) {
    window.calibre_extract = new CalibreExtract();
  }

}).call(this);
