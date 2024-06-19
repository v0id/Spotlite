<html>
<head>
  <meta http-equiv="content-type" content="text/html; charset=ISO-8859-1">
  <title>imagelist template</title>
  <style type="text/css">
      .gallerycontainer{
      position: relative;
      /*Add a height attribute and set to largest image's height to prevent overlaying*/
      }

      .thumbnail img{
      border: 1px solid white;
      margin: 0 5px 5px 0;
      vertical-align: middle;
      }

      .thumbnail:hover{
      background-color: transparent;
      }

      .thumbnail:hover img{
      border: 1px solid blue;
      }

      .thumbnail span{ /*CSS for enlarged image*/
      position: absolute;
      background-color: lightyellow;
      padding: 5px;
      left: -1000px;
      border: 1px dashed gray;
      visibility: hidden;
      color: black;
      text-decoration: none;
      }

      .thumbnail span img{ /*CSS for enlarged image*/
      border: 0;
      border-width: 0;
      padding: 2px;
      }

      .thumbnail:hover span{ /*CSS for enlarged image*/
      visibility: visible;
      top: 0;
      left: 230px; /*position where enlarged image should offset horizontally */
      z-index: 50;
      }
  </style>
  <script type="text/javascript">
      function resizeme(i)
      {
        var orig_width  = i.width;
        var orig_height = i.height;
        var max_size    = 200;

        if (i.width > i.height)
        {
            if (i.width > max_size && orig_width)
            {
                i.width  = max_size;
                i.height = orig_height * max_size / orig_width;
            }
        }
        else
        {
            if (i.height > max_size && orig_height)
            {
                i.height = max_size;
                i.width  = orig_width * max_size / orig_height;
            }
        }

        if (i.width < max_size)
        {
            var p = (max_size-i.width)/2;
            i.style.paddingRight += p;
            i.style.paddingLeft  += p;
        }
        i.style.visibility = "visible";
      }

      function resizeme_large(i)
      {
        var orig_width  = i.width;
        var orig_height = i.height;
        var max_size    = 400;

        if (i.width > i.height)
        {
            if (i.width > max_size && orig_width)
            {
                i.width  = max_size;
                i.height = orig_height * max_size / orig_width;
            }
        }
        else
        {
            if (i.height > max_size && orig_height)
            {
                i.height = max_size;
                i.width  = orig_width * max_size / orig_height;
            }
        }
      }
  </script>
</head>
<body>
<h3>Plaatjes</h3>

<div class="gallerycontainer">
{items}
</div>

</body>
</html>
