from PIL import Image
imgdir = 'D:\\stm资料\\badapple\\%d.png'
imgnum = 6570
width = 128
height = 64
outfile = 'D:\\stm资料\\badapple.txt'
f = open(outfile,'wb')
for i in range(1,imgnum+1):
	img = Image.open(imgdir % i)	# 打开图片
	img = img.convert('1')			# 转换为bilevel图像（只有黑0，白255两色）
	pixel = img.load()				# 获得像素信息
	for row in range(height//8):	# 一次性读取8行像素，所以循环次数除以8
		for x in range(width):		# 一次一列
			_byte = ''
			for y in range(row*8,row*8+8):		# 8行像素组成一个字节
				if (pixel[x, y]==255):			# 该点颜色信息
					_byte = '1' + _byte			# 最上面的像素为最低位
				else:
					_byte = '0' + _byte			# 所以从上往下看，bit放高位
			f.write(bytes([int(_byte,base=2)]))	# 转换成一个字节并写入
	print('%d...\t转换完成'%i)
f.close()