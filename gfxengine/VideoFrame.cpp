#include "VideoFrame.h"


QImage::Format VideoFrame::imageFormatFromPixelFormat(QVideoFrame::PixelFormat format)
{
    switch (format) {
    case QVideoFrame::Format_Invalid:
	return QImage::Format_Invalid;
    case QVideoFrame::Format_ARGB32:
	return QImage::Format_ARGB32;
    case QVideoFrame::Format_ARGB32_Premultiplied:
	return QImage::Format_ARGB32_Premultiplied;
    case QVideoFrame::Format_RGB32:
	return QImage::Format_RGB32;
    case QVideoFrame::Format_RGB24:
	return QImage::Format_RGB888;
    case QVideoFrame::Format_RGB565:
	return QImage::Format_RGB16;
    case QVideoFrame::Format_RGB555:
	return QImage::Format_RGB555;
    case QVideoFrame::Format_ARGB8565_Premultiplied:
	return QImage::Format_ARGB8565_Premultiplied;
    case QVideoFrame::Format_BGRA32:
    case QVideoFrame::Format_BGRA32_Premultiplied:
    case QVideoFrame::Format_BGR32:
    case QVideoFrame::Format_BGR24:
	return QImage::Format_Invalid;
    case QVideoFrame::Format_BGR565:
    case QVideoFrame::Format_BGR555:
    case QVideoFrame::Format_BGRA5658_Premultiplied:
    case QVideoFrame::Format_AYUV444:
    case QVideoFrame::Format_AYUV444_Premultiplied:
    case QVideoFrame::Format_YUV444:
    case QVideoFrame::Format_YUV420P:
    case QVideoFrame::Format_YV12:
    case QVideoFrame::Format_UYVY:
    case QVideoFrame::Format_YUYV:
    case QVideoFrame::Format_NV12:
    case QVideoFrame::Format_NV21:
    case QVideoFrame::Format_IMC1:
    case QVideoFrame::Format_IMC2:
    case QVideoFrame::Format_IMC3:
    case QVideoFrame::Format_IMC4:
    case QVideoFrame::Format_Y8:
    case QVideoFrame::Format_Y16:
	return QImage::Format_Invalid;
    case QVideoFrame::Format_User:
	return QImage::Format_Invalid;
    }
    return QImage::Format_Invalid;
}

/*!
    Returns an video pixel format equivalent to an image \a format.  If there is no equivalent
    format QVideoFrame::InvalidType is returned instead.
*/

QVideoFrame::PixelFormat VideoFrame::pixelFormatFromImageFormat(QImage::Format format)
{
    switch (format) {
    case QImage::Format_Invalid:
    case QImage::Format_Mono:
    case QImage::Format_MonoLSB:
    case QImage::Format_Indexed8:
        return QVideoFrame::Format_Invalid;
    case QImage::Format_RGB32:
        return QVideoFrame::Format_RGB32;
    case QImage::Format_ARGB32:
        return QVideoFrame::Format_ARGB32;
    case QImage::Format_ARGB32_Premultiplied:
        return QVideoFrame::Format_ARGB32_Premultiplied;
    case QImage::Format_RGB16:
        return QVideoFrame::Format_RGB565;
    case QImage::Format_ARGB8565_Premultiplied:
        return QVideoFrame::Format_ARGB8565_Premultiplied;
    case QImage::Format_RGB666:
    case QImage::Format_ARGB6666_Premultiplied:
        return QVideoFrame::Format_Invalid;
    case QImage::Format_RGB555:
        return QVideoFrame::Format_RGB555;
    case QImage::Format_ARGB8555_Premultiplied:
        return QVideoFrame::Format_Invalid;
    case QImage::Format_RGB888:
        return QVideoFrame::Format_RGB24;
    case QImage::Format_RGB444:
    case QImage::Format_ARGB4444_Premultiplied:
        return QVideoFrame::Format_Invalid;
    case QImage::NImageFormats:
        return QVideoFrame::Format_Invalid;
    }
    return QVideoFrame::Format_Invalid;
}

VideoFrame::VideoFrame()
{
	m_holdTime = -1; 
	m_captureTime = QTime(); 
	m_pixelFormat = QVideoFrame::Format_Invalid;
	m_bufferType = BUFFER_INVALID;
	m_pointer = 0;
	m_pointerLength = 0;
	m_debugPtr = false;
	m_hasTextureId = false;
	#ifdef DEBUG_VIDEOFRAME_POINTERS
	qDebug() << "VideoFrame::VideoFrame(): constructor(1): "<<this;
	#endif
}

VideoFrame::VideoFrame(int holdTime, const QTime &captureTime)
	: m_holdTime(holdTime)
	, m_captureTime(captureTime) 
	, m_pixelFormat(QVideoFrame::Format_Invalid)
	, m_bufferType(BUFFER_INVALID)
	, m_pointer(0)
	, m_pointerLength(0)
	, m_debugPtr(false)
	, m_hasTextureId(false)
{
	#ifdef DEBUG_VIDEOFRAME_POINTERS
	qDebug() << "VideoFrame::VideoFrame(): constructor(2): "<<this;
	#endif
}


VideoFrame::VideoFrame(const QImage &frame, int holdTime, const QTime &captureTime)
	: m_holdTime(holdTime)
	, m_captureTime(captureTime) 
	, m_pixelFormat(QVideoFrame::Format_Invalid)
	, m_bufferType(BUFFER_IMAGE)
	, m_image(frame)
	, m_pointer(0)
	, m_pointerLength(0)
	, m_debugPtr(false)
	, m_hasTextureId(false)
{
	#ifdef DEBUG_VIDEOFRAME_POINTERS
	qDebug() << "VideoFrame::VideoFrame(): constructor(3): "<<this;
	#endif
	
	setImage(frame);
}

VideoFrame::VideoFrame(VideoFrame *other)
	: m_holdTime(other->m_holdTime)
	, m_captureTime(other->m_captureTime)
	, m_pixelFormat(other->m_pixelFormat)
	, m_bufferType(other->m_bufferType)
	, m_image(other->m_image)
	, m_pointer(other->m_pointer)
	, m_pointerLength(other->m_pointerLength)
	, m_debugPtr(false)
	, m_hasTextureId(other->m_hasTextureId)
	, m_textureId(other->m_textureId)

{
	#ifdef DEBUG_VIDEOFRAME_POINTERS
	qDebug() << "VideoFrame::VideoFrame(): constructor(4): "<<this;
	#endif
	setSize(other->m_size);
}

VideoFrame::~VideoFrame()
{
	#ifdef DEBUG_VIDEOFRAME_POINTERS
	qDebug() << "VideoFrame::~VideoFrame(): "<<this;
	#endif
	if(m_pointer)
	{
		//#ifdef DEBUG_VIDEOFRAME_POINTERS
		if(m_debugPtr)
			qDebug() << "VideoFrame::~VideoFrame(): "<<this<<" deleting m_pointer:"<<m_pointer;
		//#endif 
		free(m_pointer);
		m_pointer = 0;
	}
}
	

uchar *VideoFrame::allocPointer(int bytes)
{
// 	if(m_pointer)
// 		delete m_pointer;
	m_pointer = (uchar*)malloc(sizeof(uchar) * bytes);
	//#ifdef DEBUG_VIDEOFRAME_POINTERS
	if(m_debugPtr)
		qDebug() << "VideoFrame::allocPointer(): "<<this<<" allocated m_pointer:"<<m_pointer<<", bytes:"<<bytes;
	m_pointerLength = bytes;
	m_bufferType = BUFFER_POINTER;
	return m_pointer;
}

bool VideoFrame::isEmpty() { return m_bufferType == BUFFER_INVALID; }
bool VideoFrame::isValid() { return m_bufferType != BUFFER_INVALID && (m_bufferType == BUFFER_IMAGE ? !m_image.isNull() : m_pointer != NULL); }

// Returns the approx memory consumption of this frame.
// 'approx' because it isn't perfect, especially when using
// BUFFER_POINTER bufferType's. 
int VideoFrame::byteSize() const
{
	int mem = 0;
	switch(m_bufferType)
	{
		case BUFFER_IMAGE:
			mem = m_image.byteCount();
			break;
		case BUFFER_POINTER:
			mem = m_pointerLength;
			break;
		default:
			break;
	}
	return mem + sizeof(VideoFrame);
}

void VideoFrame::setHoldTime(int holdTime)
{
	m_holdTime = holdTime; 
}
	
void VideoFrame::setCaptureTime(const QTime& time)
{
	m_captureTime = time;
}

void VideoFrame::setPixelFormat(QVideoFrame::PixelFormat format)
{
	m_pixelFormat = format;
}

void VideoFrame::setBufferType(BufferType type)
{
	m_bufferType = type;
}

void VideoFrame::setImage(QImage image)
{
	QImage::Format format = m_image.format();
	m_pixelFormat =
		format == QImage::Format_ARGB32 ? QVideoFrame::Format_ARGB32 :
		format == QImage::Format_RGB32  ? QVideoFrame::Format_RGB32  :
		format == QImage::Format_RGB888 ? QVideoFrame::Format_RGB24  :
		format == QImage::Format_RGB16  ? QVideoFrame::Format_RGB565 :
		format == QImage::Format_RGB555 ? QVideoFrame::Format_RGB555 :
		//format == QImage::Format_ARGB32_Premultiplied ? QVideoFrame::Format_ARGB32_Premultiplied :
		// GLVideoDrawable doesn't support premultiplied - so the format conversion below will convert it to ARGB32 automatically
		m_pixelFormat;
		
	if(m_pixelFormat == QVideoFrame::Format_Invalid)
	{
		//qDebug() << "VideoFrame: image was not in an acceptable format, converting to ARGB32 automatically.";
		m_image = m_image.convertToFormat(QImage::Format_ARGB32);
		m_pixelFormat = QVideoFrame::Format_ARGB32;
	}
	else
	{
		m_image = image;//.copy();
	}
	
	setSize(image.size());
	m_bufferType = BUFFER_IMAGE;
}

void VideoFrame::setSize(QSize newSize)
{
	m_size = newSize;
	m_rect = QRect(0,0,m_size.width(),m_size.height());
}

void VideoFrame::setRect(QRect rect)
{
	m_rect = rect;
	setSize(rect.size());
}

void VideoFrame::setPointer(uchar *dat, int len)
{
	m_bufferType = BUFFER_POINTER;
	m_pointer = dat;
	m_pointerLength = len;
}

void VideoFrame::setTextureId(GLuint id)
{
	QMutexLocker lock(&m_textureIdMutex);
	m_hasTextureId = true;
	m_textureId = id;
}

QImage VideoFrame::toImage(bool detachImage)
{
	if(!isRaw())
		return image();
		
	if(!isValid())
		return QImage();
			
	const QImage::Format imageFormat = imageFormatFromPixelFormat(pixelFormat());
	if(imageFormat != QImage::Format_Invalid)
	{
		m_image = QImage( pointer(),
				  size().width(),
				  size().height(),
				  size().width() *
					(imageFormat == QImage::Format_RGB16  ||
					 imageFormat == QImage::Format_RGB555 ||
					 imageFormat == QImage::Format_RGB444 ||
					 imageFormat == QImage::Format_ARGB4444_Premultiplied ? 2 :
					 imageFormat == QImage::Format_RGB888 ||
					 imageFormat == QImage::Format_RGB666 ||
					 imageFormat == QImage::Format_ARGB6666_Premultiplied ? 3 :
					 4),
				  imageFormat);
		
		return detachImage ? m_image.copy() : m_image;
	}
	else
	{
		qDebug() << "VideoFrame::toImage: Unable to convert pixel format to image format, cannot convert frame. Pixel Format:"<<pixelFormat();
	}
	
	return QImage();
}

