namespace gv
{
class gv_stream_android_asset : public gvi_stream
{
public:
	gv_stream_android_asset();
	~gv_stream_android_asset();
	virtual bool open(const char* name);
	virtual bool close();
	virtual gv_int tell();
	virtual void flush();
	virtual gv_int read(void* pdata, gv_int isize);
	virtual gv_int write(const void* pdata, gv_int isize);
	virtual bool seek(gv_uint pos, std::ios_base::seekdir dir);
	virtual gv_int size();
	virtual bool eof();
	virtual void set_buf_size(gv_int size);

private:
	class AAsset* _asset;
};
}
