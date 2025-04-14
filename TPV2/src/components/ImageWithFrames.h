#pragma once
#include "../ecs/Component.h"
#include "../utils/Vector2D.h"
class Transform;
class Texture;


class ImageWithFrames : public ecs::Component {
public:

	ImageWithFrames();
	ImageWithFrames(Texture* tex, float width, float height, int fil, int col, int numFrames);
	virtual ~ImageWithFrames();

	void setTexture(Texture* tex) {
		_tex = tex;
	}

	void setNumFrames(int numFrames) {
		_numFrames = numFrames;
	}

	Texture* getTexture() {
		return _tex;
	}

	Vector2D getActFrame() {
		return _actFrame;
	}

	void initComponent() override;
	//void update() override;
	void updateImage();


	//fruits necesary methods

	void resetFramePosition();
	
	void setCol(int newCol);
	void setFil(int newFil);


private:
	Transform* _tr;
	Texture* _tex;

	float _width;
	float _height;

	Vector2D _actFrame;

	float _texWidthMax;
	float _texHeigthMax;


	float _timer;
	int _initialFil;
	int _initialCol;
	int _numFrames;
	int _actualFrame;

	bool _reverseAnimation;
};

