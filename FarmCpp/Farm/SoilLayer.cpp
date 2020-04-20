#include "SoilLayer.h"
#include <cocos/2d/CCSprite.h>
#include <cocos/2d/CCTMXLayer.h>


// tile���� ת�� ��Ƭ�������ĵ�OpenGL����
Vec2 positionForTileCoord(TMXTiledMap*tiledMap, Vec2 position) {
	auto mapSize = tiledMap->getMapSize();
	auto tileSize = tiledMap->getTileSize();
	auto tileWidth = tiledMap->getBoundingBox().size.width / tiledMap->getMapSize().width;
	auto tileHeight = tiledMap->getBoundingBox().size.height / tiledMap->getMapSize().height;

	auto variable1 = (position.x + mapSize.width / 2 - mapSize.height) * tileWidth * tileHeight;
	auto variable2 = (-position.y + mapSize.width / 2 + mapSize.height) * tileWidth * tileHeight;

	float posx = (variable1 + variable2) / 2 / tileHeight;
	float posy = (variable2 - variable1) / 2 / tileWidth;

	return Point(posx, posy);
}


SoilLayer::SoilLayer() {
}

SoilLayer::~SoilLayer() {
}


bool SoilLayer::init() {
	m_pTiledMap = TMXTiledMap::create("farm_map/farm.tmx");
	//m_pTiledMap->setPosition(50, 150);
	m_pTiledMap->setAnchorPoint(Vec2::ZERO);
	m_pTiledMap->setPosition(Vec2(380, -135));
	this->addChild(m_pTiledMap);

	return true;
}

void SoilLayer::update() {

}


SoilPtr SoilLayer::getClickingSoil(const Vec2& loc) {
	SoilPtr soil = nullptr;
	auto it = std::find_if(m_soils.begin(), m_soils.end(), [&loc](SoilPtr soil) {
		//���ζԽ��ߴ�С
		auto size = soil->getContentSize();
		auto soilPos = soil->getPosition();
		//��������ı任
		auto pos = loc - soilPos;
		//������εĴ�С��һ��
		auto halfOfArea = size.width * size.height / 4;
		//��������������С
		auto clickOfArea = fabs(pos.x * size.height * 0.5f) + fabs(pos.y * size.width * 0.5f);
		//�ж��Ƿ���������
		return clickOfArea <= halfOfArea;

	});

	if (it != m_soils.end())
		soil = *it;

	return soil;
}


Sprite* SoilLayer::updateSoil(int soilID, int soilLv) {
	//��ȡ��������������
	auto layer = m_pTiledMap->getLayer("soil layer");

	//�ҵ���Ӧ����������
	auto mapSize = m_pTiledMap->getMapSize();
	int width = (int)mapSize.width;

	int x = soilID % width;
	int y = soilID / width;

	auto tileCoordinate = Vec2((float)x, (float)y);
	//���ݵȼ�������ͼ
	int gid = 1 + soilLv;
	layer->setTileGID(gid, tileCoordinate);

	return layer->getTileAt(tileCoordinate);
}

SoilPtr SoilLayer::addSoil(int soilID, int soilLv) {
	//���²������������
	auto sprite = this->updateSoil(soilID, soilLv);
	SoilPtr soil = Soil::create(sprite, soilID, soilLv);

	m_soils.push_back(soil);
	this->addChild(soil);
	//����λ�� ��ǰλ���Ѿ�����������
	auto pos = m_pTiledMap->convertToWorldSpace(sprite->getPosition());

	soil->setPosition(pos);

	return soil;
}

void SoilLayer::addCrop(int row, int col, const std::string&strCropName) {
	auto pos = positionForTileCoord(m_pTiledMap, Vec2((float)row, (float)col));
	pos = pos + Vec2(370, -155);
	ui::Button* spriteCrop = ui::Button::create(strCropName, "", "", ui::Widget::TextureResType::PLIST);
	spriteCrop->setName(strCropName);
	spriteCrop->setPosition(pos);
	spriteCrop->addClickEventListener([](Ref* ref) {
		ui::Button*self = (ui::Button*)ref;
		log("click %s", self->getName().c_str());
	});
	this->addChild(spriteCrop);
}