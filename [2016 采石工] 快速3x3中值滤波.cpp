#include <cv.h>
#include <highgui.h>

/*
///	编写者： laviewpbt,  编写时间： 2015.10.16， 联系QQ: 33184777
/// 优化cmp: 落羽の殇  联系QQ:200759103

作者：采石工
Q Q：544617183
邮箱：quarrying@qq.com
博客：http://www.cnblogs.com/quarryman/
更新时间：2016年04月22日
如有错误和建议，欢迎发邮件或留言！引用请保留上述信息！

参考：
OpenCV medianBlur_SortNet
http://ndevilla.free.fr/median/median/index.html
https://en.wikipedia.org/wiki/Median_of_medians
https://en.wikipedia.org/wiki/Quickselect
*/

static unsigned int* pixGreater = NULL;
static unsigned int* pixLess = NULL;
static unsigned int cmpTable[256 * 256 * 2] = { 0 };
struct autoCmpTable
{
	autoCmpTable() {
		unsigned int x, y, p;
		unsigned int tableLength = 256 * 256;
		pixGreater = cmpTable;
		pixLess = cmpTable + tableLength;
		for (x = 0; x < 256; x++) {
			for (y = 0; y < 256; y++) {
				p = x + y * 256;
				if (x > y)
				{
					pixLess[p] = x;
					pixGreater[p] = y;
				}
				else
				{
					pixGreater[p] = x;
					pixLess[p] = y;
				}
			}
		}
	}
};
static autoCmpTable initCmpTable;

const uchar g_Saturate8u[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
	64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
	80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
	96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
	128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
	144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
	176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
	192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
	208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
	224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255
};
#define CV_FAST_CAST_8U(t)   (assert(-256 <= (t) && (t) <= 512), g_Saturate8u[(t)+256])
#define CV_MIN_8U(a,b)       ((a) - CV_FAST_CAST_8U((a) - (b)))
#define CV_MAX_8U(a,b)       ((a) + CV_FAST_CAST_8U((b) - (a)))

//#define ICV_Swap(a, b)\
//if ((a) > (b)){ (a) ^= (b); (b) ^= (a); (a) ^= (b); }

//#define ICV_Swap(a, b)\
//{int t = CV_FAST_CAST_8U(a - b); b += t; a -= t; }

#define ICV_Swap(a, b)\
{int pos = a + (b << 8); b = pixLess[pos]; a = pixGreater[pos]; }

// 因为只是为了验证正确性，下面代码只能处理单通道八位图像，且边界处未处理
void kcvMedian3x3(const IplImage* src, IplImage* dst)
{
	if ((src->width != dst->width) || (src->height != dst->height))
	{
		CV_Error(CV_StsUnmatchedSizes, "Unmatched sizes");
	}
	if ((src->depth != IPL_DEPTH_8U) || (dst->depth != IPL_DEPTH_8U))
	{
		CV_Error(CV_BadDepth, "Bad depth");
	}
	if ((src->nChannels != 1) || (dst->nChannels != 1))
	{
		CV_Error(CV_BadNumChannels, "Bad nChannels");
	}
	if ((src->imageData == NULL) || (dst->imageData == NULL))
	{
		CV_Error(CV_BadDataPtr, "Null image data");
	}

	int w = src->width;
	int h = src->height;
	int sstep = src->widthStep;
	int dstep = dst->widthStep;
	const uchar* sptr = (uchar*)src->imageData;
	uchar* dptr = (uchar*)dst->imageData;

	sstep /= sizeof(sptr[0]);
	dstep /= sizeof(dptr[0]);
	const uchar* sptrPrev = sptr;
	const uchar* sptrCurr = sptr + sstep;
	const uchar* sptrNext = sptr + sstep * 2;
	uchar* dptrCurr = dptr + dstep;
	uchar p0, p1, p2, p3, p4, p5, p6, p7, p8;
	for (int i = 1; i < h - 1; ++i)
	{
		for (int j = 1; j < w - 1; ++j)
		{
			p0 = sptrPrev[j - 1], p1 = sptrPrev[j], p2 = sptrPrev[j + 1];
			p3 = sptrCurr[j - 1], p4 = sptrCurr[j], p5 = sptrCurr[j + 1];
			p6 = sptrNext[j - 1], p7 = sptrNext[j], p8 = sptrNext[j + 1];

			ICV_Swap(p1, p2); ICV_Swap(p0, p1); ICV_Swap(p1, p2);
			ICV_Swap(p4, p5); ICV_Swap(p3, p4); ICV_Swap(p4, p5);
			ICV_Swap(p7, p8); ICV_Swap(p6, p7); ICV_Swap(p7, p8);
			ICV_Swap(p0, p3); ICV_Swap(p3, p6);
			ICV_Swap(p5, p8); ICV_Swap(p2, p5);
			ICV_Swap(p4, p7); ICV_Swap(p1, p4); ICV_Swap(p4, p7);
			ICV_Swap(p4, p2); ICV_Swap(p6, p4); ICV_Swap(p4, p2);

			dptrCurr[j] = p4;
		}
		sptrPrev += sstep;
		sptrCurr += sstep;
		sptrNext += sstep;
		dptrCurr += dstep;
	}
}

struct MinMax8u
{
	typedef uchar value_type;
	typedef int arg_type;
	enum { SIZE = 1 };
	arg_type load(const uchar* ptr) { return *ptr; }
	void store(uchar* ptr, arg_type val) { *ptr = (uchar)val; }
	void operator()(arg_type& a, arg_type& b) const
	{
		int t = CV_FAST_CAST_8U(a - b);
		b += t; a -= t;
	}
};

struct MinMaxVec8u
{
	typedef uchar value_type;
	typedef __m128i arg_type;
	enum { SIZE = 16 };
	arg_type load(const uchar* ptr) { return _mm_loadu_si128((const __m128i*)ptr); }
	void store(uchar* ptr, arg_type val) { _mm_storeu_si128((__m128i*)ptr, val); }
	void operator()(arg_type& a, arg_type& b) const
	{
		arg_type t = a;
		a = _mm_min_epu8(a, b);
		b = _mm_max_epu8(b, t);
	}
};

// 代码改自OpenCV中的medianBlur_SortNet
void kcvMedian3x3_v2(const IplImage* src, IplImage* dst)
{
	if ((src->width != dst->width) || (src->height != dst->height))
	{
		CV_Error(CV_StsUnmatchedSizes, "Unmatched sizes");
	}
	if ((src->depth != IPL_DEPTH_8U) || (dst->depth != IPL_DEPTH_8U))
	{
		CV_Error(CV_BadDepth, "Bad depth");
	}
	if (src->nChannels != dst->nChannels)
	{
		CV_Error(CV_BadNumChannels, "Bad nChannels");
	}
	if ((src->imageData == NULL) || (dst->imageData == NULL))
	{
		CV_Error(CV_BadDataPtr, "Null image data");
	}

	int w = src->width;
	int h = src->height;
	int cn = src->nChannels;
	int sstep = src->widthStep;
	int dstep = dst->widthStep;
	const uchar* sptr = (uchar*)src->imageData;
	uchar* dptr = (uchar*)dst->imageData;

	sstep /= sizeof(sptr[0]);
	dstep /= sizeof(dptr[0]);

	typedef MinMax8u Op;
	typedef MinMaxVec8u VecOp;
	typedef Op::value_type T;
	typedef Op::arg_type WT;
	typedef VecOp::arg_type VT;

	int i, j;
	Op op;
	VecOp vop;

	if (w == 1 || h == 1)
	{
		int len = w + h - 1;
		int sdelta = h == 1 ? cn : sstep;
		int sdelta0 = h == 1 ? 0 : sstep - cn;
		int ddelta = h == 1 ? cn : dstep;

		for (i = 0; i < len; i++, sptr += sdelta0, dptr += ddelta)
		for (j = 0; j < cn; j++, sptr++)
		{
			WT p0 = sptr[i > 0 ? -sdelta : 0];
			WT p1 = sptr[0];
			WT p2 = sptr[i < len - 1 ? sdelta : 0];

			op(p0, p1); op(p1, p2); op(p0, p1);
			dptr[j] = (T)p1;
		}
		return;
	}

	w *= cn;
	for (i = 0; i < h; i++, dptr += dstep)
	{
		const T* row0 = sptr + MAX(i - 1, 0)*sstep;
		const T* row1 = sptr + i*sstep;
		const T* row2 = sptr + MIN(i + 1, h - 1)*sstep;
		int limit = cn;

		for (j = 0;;)
		{
			for (; j < limit; j++)
			{
				int j0 = j >= cn ? j - cn : j;
				int j2 = j < w - cn ? j + cn : j;
				WT p0 = row0[j0], p1 = row0[j], p2 = row0[j2];
				WT p3 = row1[j0], p4 = row1[j], p5 = row1[j2];
				WT p6 = row2[j0], p7 = row2[j], p8 = row2[j2];

				op(p1, p2); op(p4, p5); op(p7, p8); op(p0, p1);
				op(p3, p4); op(p6, p7); op(p1, p2); op(p4, p5);
				op(p7, p8); op(p0, p3); op(p5, p8); op(p4, p7);
				op(p3, p6); op(p1, p4); op(p2, p5); op(p4, p7);
				op(p4, p2); op(p6, p4); op(p4, p2);
				dptr[j] = (T)p4;
			}

			if (limit == w)
				break;

			for (; j <= w - VecOp::SIZE - cn; j += VecOp::SIZE)
			{
				VT p0 = vop.load(row0 + j - cn), p1 = vop.load(row0 + j), p2 = vop.load(row0 + j + cn);
				VT p3 = vop.load(row1 + j - cn), p4 = vop.load(row1 + j), p5 = vop.load(row1 + j + cn);
				VT p6 = vop.load(row2 + j - cn), p7 = vop.load(row2 + j), p8 = vop.load(row2 + j + cn);

				vop(p1, p2); vop(p4, p5); vop(p7, p8); vop(p0, p1);
				vop(p3, p4); vop(p6, p7); vop(p1, p2); vop(p4, p5);
				vop(p7, p8); vop(p0, p3); vop(p5, p8); vop(p4, p7);
				vop(p3, p6); vop(p1, p4); vop(p2, p5); vop(p4, p7);
				vop(p4, p2); vop(p6, p4); vop(p4, p2);
				vop.store(dptr + j, p4);
			}

			limit = w;
		}
	}
}

template<typename Func>
double kcvBench_(const Func& func, unsigned int iterNum = 1)
{
	auto t1 = cvGetTickCount();
	while (iterNum--) func();
	auto t2 = cvGetTickCount();
	return (t2 - t1) / (1000.0 * cvGetTickFrequency());
}

int main()
{
	IplImage* src = cvLoadImage("lena.jpg", 0);
	IplImage* dst1 = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage* dst2 = cvCreateImage(cvGetSize(src), 8, 1);
	IplImage* dst3 = cvCreateImage(cvGetSize(src), 8, 1);
	cvShowImage("src", src);

	int iterNum = 100;
	for (int i = 0; i < iterNum; ++i)
	{
		printf("CV_MEDIAN %f ms\n", kcvBench_([=](){cvSmooth(src, dst1, CV_MEDIAN, 3); }));
		printf("kcvMedian3x3 %f ms\n", kcvBench_([=](){kcvMedian3x3(src, dst2); }));
		printf("kcvMedian3x3_v2 %f ms\n", kcvBench_([=](){kcvMedian3x3_v2(src, dst3); }));
		printf("\n");
	}

	cvShowImage("CV_MEDIAN", dst1);
	cvShowImage("kcvMedian3x3", dst2);
	cvShowImage("kcvMedian3x3_v2", dst3);

	cvSetImageROI(dst1, cvRect(1, 1, src->width - 2, src->height - 2));
	cvSetImageROI(dst2, cvRect(1, 1, src->width - 2, src->height - 2));
	cvAbsDiff(dst1, dst2, dst2);
	printf("Different Pixels: %d\n", cvCountNonZero(dst2));
	cvResetImageROI(dst1);
	cvResetImageROI(dst2);
	
	cvAbsDiff(dst1, dst3, dst3);
	printf("Different Pixels: %d\n", cvCountNonZero(dst3));

	cvWaitKey(0);
	cvReleaseImage(&src);
	cvReleaseImage(&dst1);
	cvReleaseImage(&dst2);
	cvReleaseImage(&dst3);
	return 0;
}
