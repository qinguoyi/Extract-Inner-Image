# Extract-inner-image
This project can extract inner image of something like ring, but not only ring. Besides, you can counting according to the result image of inner image.
Configuration environment : OpenCV3.1.0 + VS2013

* **Simple-Example** 

In this example, we show some simple images of some rings which are Scattered. In the algorithm, we find the contours, and then use the AND, OR relationship to extract the inner ring image. 

<div align=center><img width="240" height="170" src="https://github.com/qinguoyi/Extract-inner-image/blob/master/Complex-Example/src.png"/></div> ![](https://github.com/qinguoyi/Extract-inner-image/blob/master/Simple-Example/dst.png)

* **Complex-Example**

In this example, we show some complex images of some rings which are Intensive. If we still use a simple method to process the image, some gap images will be extracted as inner loops.Then we improved the method,  we use the distance transform and watershed algorithm to elimate this error.

<div align=center><img width="240" height="170" src="https://github.com/qinguoyi/Extract-inner-image/blob/master/Complex-Example/dst.png"/></div>


* **More Details**

More algorithm details please visit [my blog](https://www.cnblogs.com/qinguoyi/p/8325010.html)
