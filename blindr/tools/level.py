#!/usr/bin/python
import tmx
from lputil import *

FLOOR_TILES = [0,1,10]
BOX_TILES = []#10]

def main():
	# load the map
	tm = tmx.TileMap('assets/maps/level.tmx')


	# identify the colliders
	w,h = tm.size
	
	def is_floor(x,y):
		lid = x + y * w
		for layer in tm.tilelayers:
			tile = layer.tiles[lid]
			if tile is not None and tile.id in FLOOR_TILES:
				return True
		return False

	def is_box(x,y):
		lid = x + y * w
		for layer in tm.tilelayers:
			tile = layer.tiles[lid]
			if tile is not None and tile.id in BOX_TILES:
				return True
		return False

	# print a collision proof
	li = ['`'] * (w * h)
	for y,x in xyrange(h, w):
		lid = x + y * w
		if is_floor(x,y):
			li[lid] = 'F'
		elif is_box(x,y):
			li[lid] = 'B'
	proof = []
	for y in xrange(h):
		proof.append("".join(map(str,li[w*y : w*(y+1)])))
	print '\n'.join(proof)

	# compute floors
	floors = []
	for y in xrange(h):
		x = 0
		while x < w:
			while x < w and not is_floor(x,y):
				x+=1
			if x < w:
				x0 = x
				x+=1
				while x < w and is_floor(x,y):
					x+=1
				floors.append( (x0, y, x-x0) )

	# compute boxes
	boxes = [ (x,y) for y,x in xyrange(h,w) if is_box(x,y) ]

	# write datas
	print "Writing data..."
	with open('source/BlindrData.cpp', 'w') as f:
		f.write('#include "Blindr.h"\n')
		f.write('int Blindr::kLevelW = %d;\n' % w);
		f.write('int Blindr::kLevelH = %d;\n' % h);

		f.write('int Blindr::kFloorCount = %d;\n' % len(floors))
		f.write('int Blindr::kBoxCount = %d;\n' % len(boxes))
		f.write('Blindr::FloorData Blindr::kFloors[] = {\n')
		for x,y,w in floors:
			f.write('\t{%d,%d,%d},\n' % (x,y,w))
		f.write('};\n')
		f.write('Blindr::BoxData Blindr::kBoxes[] = {\n')
		for x,y in boxes:
			f.write('\t{%d,%d},\n' % (x,y))
		f.write('};\n')




if __name__ == '__main__' : main()