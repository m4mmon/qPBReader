/*
 based on an answer from Tim Down
 http://stackoverflow.com/questions/298750/how-do-i-select-text-nodes-with-jquery
*/
function qPBReaderGetTextNodesIn(node, includeWhitespaceNodes)
{
   var textNodes = [], nonWhitespaceMatcher = /\S/;

   function getTextNodes(node)
   {
      if (node.nodeType == Node.TEXT_NODE)
      {
         if (includeWhitespaceNodes || nonWhitespaceMatcher.test(node.nodeValue))
         {
            textNodes.push(node);
         }
      }
      else
      {
         var nbChildren = node.childNodes.length;
         for (var idx = 0; idx < nbChildren; ++idx)
         {
            getTextNodes(node.childNodes[idx]);
         }
      }
   }

   getTextNodes(node);
   return textNodes;
}

/*
 based on an answer from aaron
 http://stackoverflow.com/questions/5143534/get-the-position-of-text-within-an-element
*/
function qPBReaderFindClickedWord(elt, x, y)
{
   var textNodes = qPBReaderGetTextNodesIn(elt, true);
   window.py_bridge.debug('Got ' + textNodes.length + ' text nodes');
   for (var idx = 0; idx < textNodes.length; idx++)
   {
      window.py_bridge.debug('Checking for match in node #' + idx);
      var textNode = textNodes[idx];
      window.py_bridge.debug('nodeName [' + textNode.nodeName + ']');
      window.py_bridge.debug('[' + textNode.textContent + ']')
      var range = document.createRange();
      var reg=new RegExp('[ \W\t\r\n\f, ’\.]');
      var words = textNode.textContent.split(reg);
      var start = 0;
      var end = 0;
      for (var jdx = 0; jdx < words.length; jdx++)
      {
         var word = words[jdx];
         window.py_bridge.debug('Processing word #' + jdx +  ' [' + word + ']');
         end = start + word.length;
         range.setStart(textNode, start);
         range.setEnd(textNode, end);
         // not getBoundingClientRect as word could wrap
         var rects = range.getClientRects();
         //window.py_bridge.debug('rect(' + rects[0].left + ', ' + rects[0].top + ', ' + rects[0].right + ', ' + rects[0].bottom + ')');
         var clickedRect = isClickInRects(rects);
         if (clickedRect)
         {
            window.py_bridge.debug('found!');
            return [word,
                    clickedRect.left,
                    clickedRect.top,
                    clickedRect.right,
                    clickedRect.bottom]
         }
         start = end + 1;
      }
      window.py_bridge.debug('Not matched');
   }

   function isClickInRects(rects)
   {
      for (var idx = 0; idx < rects.length; idx++)
      {
         var r = rects[idx];
         if (r.left < x && r.right > x && r.top < y && r.bottom > y)
         {
            return r;
         }
      }
      return false;
   }

   return null;
}
