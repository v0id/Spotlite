<html>
<head>
  <meta http-equiv="content-type" content="text/html; charset=ISO-8859-1">
  <title>spot template</title>
  <style>
      td
      {
        font-size: small;
        vertical-align: top;
      }
      .file
      {
        color: green;
        white-space: nowrap;
      }
      .spotter
      {
        color: navy;
      }
      .friend
      {
        color: green;
      }
      .username
      {
        text-decoration: none;
        color: black;
        font-weight: bold;
      }
      .spottername
      {
        text-decoration: none;
        color: black;
      }
      .commentstable
      {
        width: 100%;
        border-collapse:collapse;
      }
      .commentheader
      {
        font-size: small;
        border-top: 1px solid black;
        padding-top: 5px;
      }
      .comment
      {
        font-size: medium;
      }
      .commentheader, .comment
      {
        padding-bottom: 1em;
      }
      #box
      {
          position: relative;
          margin-left: auto;
          margin-right: auto;
          margin-top: 1em;
          padding: 0;
          text-align: left;
          width:99%;
          background-color: #eeeeee;;
      }
      #content
      {
          padding:2em;
          font-size: small;
      }
      #tlc, #trc, #blc, #brc
      {
          background-color: transparent;
          background-repeat: no-repeat;
      }
      #tlc
      {
          background-image:url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAQCAAAAAAzcwDHAAAAFnRFWHRTb2Z0d2FyZQBnaWYycG5nIDIuNS4yopwtOQAAAFpJREFUGJXNy6ENgDAURdHbhqQrYFFIVkCyAZOSIJGMwAjI78oLotgfygA8eXJfKNSLH/ZHbADIh6437ta3vgwFNsbqvjJBVvJo5wxGksdlEEqS05jvDiFw5QMZnhrB8P3HWwAAAABJRU5ErkJggg==);
          background-position: 0% 0%;
      }
      #trc
      {
          background-image:url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAB4AAAAQCAAAAAAkUZAOAAAAFnRFWHRTb2Z0d2FyZQBnaWYycG5nIDIuNS4yopwtOQAAAFxJREFUGJVj/M+ADzDhlR2+0iwHkDgcDPqcqNKM35B5L24KWKDK/0cDx1F4qLoZGDgZdngg2/0TzmT/yf6TnYHhxQcBrC7/yfCT/ef3D/pbcXrsJ8NPBsXf3xECAM5oM3HvLSjVAAAAAElFTkSuQmCC);
          background-position: 100% 0%;
      }
      #blc
      {
          background-image:url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAQCAAAAAAzcwDHAAAAFnRFWHRTb2Z0d2FyZQBnaWYycG5nIDIuNS4yopwtOQAAAJlJREFUGJVljbEOwVAYhb9Ww72mDhJMPIKxj+NhPIjHsNrYbBikE7kTleCeioStetvzb1++858oX84NPYLEnnZiC1C2TK9mPbECEeIYSo8apkcq09BNLPvJS6ZnZf6Q4XbMu9u/GyocXdcrkxkw9Z92OnKbjGCrs+DzOF80CNZJZzfcyT1rMPqi/LiTFEJUuEPhGxBUCG+r+wFokUCUdAifcQAAAABJRU5ErkJggg==);
          background-position: 0% 100%;
      }
      #brc
      {
          background-image:url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAB4AAAAQCAAAAAAkUZAOAAAAFnRFWHRTb2Z0d2FyZQBnaWYycG5nIDIuNS4yopwtOQAAAJ1JREFUGJV9jjEOwjAMRX9RBkddMmZMxRDGHomjcQnuUBZGdiaYUBdqm4kBUdmi1Jm+nv7Lbx5wp5DDvsxxg99jrOL4FyuE19rio8cqYBUjD84MVgEZeVDjZRVM19G2719IAkwvyKliAX/6ggEdGTz6qYPkkpanAbcztn227aOh7bPNu5rt360JlKivndUFuzPmkm0XaC6R5wdK5CjeqKE0jACt6LYAAAAASUVORK5CYII=);
          background-position: 100% 100%;
      }
      #tb, #bb
      {
          background-color: transparent;
          background-repeat: repeat-x;
      }
      #tb
      {
          background-image:url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAQCAAAAAAzcwDHAAAAFnRFWHRTb2Z0d2FyZQBnaWYycG5nIDIuNS4yopwtOQAAACVJREFUGJVj/M+ACZiwiA1GQZYDWAQZv2ETxOZNlu/YBH9S5CQAPywFyHpdC2gAAAAASUVORK5CYII=);
          background-position: 0% 0%;
      }
      #bb
      {
          background-image:url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAQCAAAAAAzcwDHAAAAFnRFWHRTb2Z0d2FyZQBnaWYycG5nIDIuNS4yopwtOQAAAC1JREFUGJVjfMeACZiwiNFPkOUnNsEXRAt+INqirdgEubEJqmMRZLzG+R0DAgBKrQ+a1F+igAAAAABJRU5ErkJggg==);
          background-position: 50% 100%;
      }
      #rb
      {
          background-image:url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAB4AAAAKCAAAAACGHLHzAAAAFnRFWHRTb2Z0d2FyZQBnaWYycG5nIDIuNS4yopwtOQAAAChJREFUGJVjfMeACr79XpCgAOcxocuictGlufBLE9DNgF+aMsOp6jQADvcICTGADCIAAAAASUVORK5CYII=);
          background-position: 100% 0%;
          background-repeat: repeat-y;
      }
      #lb
      {
          background-image:url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAKCAAAAACRPiE6AAAAFnRFWHRTb2Z0d2FyZQBnaWYycG5nIDIuNS4yopwtOQAAAB9JREFUCJljvL8ggZWLARUwMTAwMHzDJkgLleiAQu0AZTAF+Haa6t0AAAAASUVORK5CYII=);
          background-position: 0% 100%;
          background-repeat: repeat-y;
      }
  </style>
  <script type="text/javascript">

  var orig_width = 0, orig_height = 0, max_size = 500;

  function resizeme(i)
  {
    orig_width  = i.width;
    orig_height = i.height;
    max_size2   = (max_size > screen.width ? screen.width-75 : max_size);

    if (i.width > i.height)
    {
        if (i.width > max_size2 && orig_width)
        {
            i.width  = max_size2;
            i.height = orig_height * max_size2 / orig_width;
        }
    }
    else
    {
        if (i.height > max_size2 && orig_height)
        {
            i.height = max_size2;
            i.width  = orig_width * max_size2 / orig_height;
        }
    }
  }

  function supersizeme(i)
  {
    if (i.width != orig_width)
    {
        i.width  = orig_width;
        i.height = orig_height;
    }
  }
  </script>
</head>
<body>

<div id="box">

    <div id="lb">
    <div id="rb">
    <div id="bb"><div id="blc"><div id="brc">
    <div id="tb"><div id="tlc"><div id="trc">

    <div id="content">{description}</div></b></i>
    <br clear="all">

    </div></div></div></div>
    </div></div></div></div>
</div>


<br>
<br>
<b><u>Informatie:</u></b>

<table>
<tr><td><b>Weblink</b><td>:<td> {website}
<tr><td><b>Grootte</b><td>:<td> {size}
<tr><td><b>Aantal&nbsp;bestanden</b><td>:<td> {numfiles}
<tr><td><b>Bestandsnaam</b><td>:<td> <span class="file">{firstfile}</span> t/m <span class="file">{lastfile}</span>
<tr><td><b>Nieuwsgroep</b><td>:<td> {newsgroup}
<tr><td><b>Categorie&euml;n</b><td>:<td> {categories}
<tr><td><b>Spotter</b><td>:<td> {spotterlink}
<tr><td><b>Tag</b><td>:<td> {tag} {taglink}
</table>
<br>

<table class="commentstable">
{comments}
</table>

</body>
</html>
