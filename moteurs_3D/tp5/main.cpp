#include <osgViewer/Viewer>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osg/PositionAttitudeTransform>
#include <osgGA/NodeTrackerManipulator>
#include <osgGA/GUIEventHandler>
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Switch>
#include <osg/Camera>
#include <osg/Fog>
#include <osgParticle/PrecipitationEffect>
#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowMap>
#include <osgShadow/SoftShadowMap>
#include <osgGA/DriveManipulator>
#include <osg/NodeCallback>
#include <osgUtil/LineSegmentIntersector>
#include <osg/TexMat>
#include <osgSim/DOFTransform>
#include <osgParticle/SmokeEffect>
#include <osgParticle/ExplosionEffect>
#include <iostream>
#include <random>

class SearchNode : public osg::NodeVisitor
{
public:
    SearchNode(const std::string &name) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN), _name(name) {}

    virtual void apply(osg::Node &n) {
        if (n.getName() == _name)
            _node = &n;
        else
            traverse(n);
    }

    inline osg::Node *node() const { return _node; }

protected:
    std::string _name;
    osg::ref_ptr<osg::Node> _node;
};

class RemoveNodeByName : public osg::NodeVisitor
{
public:
    RemoveNodeByName(const std::string &name) : _name(name) {}

    virtual void apply(osg::Group &g) {
        for (size_t i = 0; i < g.getNumChildren(); ++i)
            if (g.getChild(i)->getName() == _name)
                g.removeChild(i);

        traverse(g);
    }

private:
    std::string _name;
};

class EventHandler : public osgGA::GUIEventHandler
{
public:
    EventHandler(osg::ref_ptr<osg::StateSet> state, osg::ref_ptr<osg::Group> scene) : _state(state), _scene(scene), _degree(0) {
        SearchNode searchTourelle("turret");
        _scene->accept(searchTourelle);
        _tourelleDOF = dynamic_cast<osgSim::DOFTransform*>(searchTourelle.node());

        SearchNode searchTankPat("tank_pat");
        _scene->accept(searchTankPat);
        _tank_pat = dynamic_cast<osg::PositionAttitudeTransform *>(searchTankPat.node());

    }

    virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa) {
        switch (ea.getEventType()) {
        case osgGA::GUIEventAdapter::KEYDOWN:
            switch (ea.getKey()) {
            case 'a':
                turnTurret(-2.f);
                break;
            case 'z':
                turnTurret(2.f);
                break;
            case 'f':
                fire();
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }

        return false;
    }

private:
    void turnTurret(float degree) {
        _degree += degree;

        auto hpr = _tourelleDOF->getCurrentHPR();
        hpr[0] = osg::DegreesToRadians(_degree);
        _tourelleDOF->setCurrentHPR(hpr);
    }

    void fire() {
        osg::ref_ptr<osgParticle::ExplosionEffect> effectNode = new osgParticle::ExplosionEffect;
        effectNode->setTextureFileName("fire.png");
        effectNode->setIntensity(2);
        effectNode->setScale(4);

        auto pos = _tank_pat->getPosition();
        pos.y() += 10;

        osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
        pat->setPosition(pos);
        pat->setAttitude(osg::Quat(osg::DegreesToRadians(_degree), osg::Vec3{0.f, 0.f, 1.f}));
        //pat->addChild(effectNode->getParticleSystem());
        pat->addChild(effectNode->getEmitter());

        _scene->addChild(pat);
        _scene->addChild(effectNode);
    }

    void fire() {
        osg::ref_ptr<osgParticle::ExplosionEffect> explosion = new osgParticle::ExplosionEffect;
        explosion->setName("tank_fire");
        explosion->setTextureFileName("feu.png");
        explosion->setIntensity(2);
        explosion->setScale(4);
        explosion->setPosition(osg::Vec3(10,20,0));
        _scene->addChild(explosion);
    }

private:
    osg::ref_ptr<osg::StateSet> _state;
    osg::ref_ptr<osg::Group> _scene;
    float _degree;
    osgSim::DOFTransform * _tourelleDOF;
    osg::PositionAttitudeTransform * _tank_pat;
};

osg::Node* creation_terrain()
{
    osg::ref_ptr<osg::Image> image = osgDB::readImageFile("terrain.tga");

    osg::ref_ptr<osg::HeightField> field = new osg::HeightField;
    field->allocate(image->s(), image->t());
    for (size_t y = 0; y < image->t(); ++y)
        for (size_t x = 0; x < image->s(); ++x)
            field->setHeight(x, y, (*image->data(x, y) / 255.f) * 30.f);

    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    texture->setImage(osgDB::readImageFile("herbe.tga"));
    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

    osg::Matrixd matrix;
    matrix.makeScale(10, 10, 1);

    osg::ref_ptr<osg::TexMat> matTexture = new osg::TexMat;
    matTexture->setMatrix(matrix);

    osg::ref_ptr<osg::ShapeDrawable> shapeDrawable = new osg::ShapeDrawable(field);
    shapeDrawable->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
    shapeDrawable->getOrCreateStateSet()->setTextureAttributeAndModes(0, matTexture, osg::StateAttribute::ON);

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(shapeDrawable);

    return geode.release();
}

bool intersection_terrain(float x, float y, osg::Node * terrain, osg::Vec3 & pos, osg::Vec3 & normale)
{
    osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(osg::Vec3(x, y, 1000), osg::Vec3(x, y, -1000));
    osgUtil::IntersectionVisitor iv(intersector);
    terrain->accept(iv);
    if (intersector->containsIntersections()) {
        osgUtil::LineSegmentIntersector::Intersections & intersections = intersector->getIntersections();
        auto intersection = *intersections.begin();
        pos = intersection.getWorldIntersectPoint();
        normale = intersection.getWorldIntersectNormal();
        return true;
    }
    return false;
}

class AnimationTank : public osg::AnimationPathCallback
{
public:
    AnimationTank(osg::AnimationPath * p, osg::Node * terrain, osg::Node * tank) : osg::AnimationPathCallback(p), _terrain(terrain), _tank(tank) {}

    virtual void operator ()(osg::Node * n, osg::NodeVisitor * nv) {
        osg::AnimationPathCallback::operator ()(n, nv);
        osg::PositionAttitudeTransform * transform = dynamic_cast<osg::PositionAttitudeTransform*>(n);
        if (transform) {
            osg::Vec3f pos = transform->getPosition();
            osg::Vec3f normale;

            osg::Vec3f dir = pos - _lastPos;
            // angle entre dir et x ou y puis rotate

            intersection_terrain(pos.x(), pos.y(), _terrain, pos, normale);
            osg::Quat rotate;
            rotate.makeRotate({0, 0, 1}, normale);

            transform->setPosition(pos);
            transform->setAttitude(rotate);

            SearchNode sSmoke("tank_smoke");
            _tank->accept(sSmoke);
            osgParticle::ParticleEffect * smoke = dynamic_cast<osgParticle::ParticleEffect*>(sSmoke.node());
            if (smoke) {
                smoke->setPosition(pos);
            }

            _lastPos = pos;
        }
    }

private:
    osg::Node * _terrain;
    osg::Node * _tank;
    osg::Vec3f _lastPos;
};

osg::Group * creation_foret(osg::Node * terrain, size_t nb_arbres)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> disX(terrain->asGeode()->getBoundingBox().xMin(), terrain->asGeode()->getBoundingBox().xMax());
    std::uniform_real_distribution<float> disY(terrain->asGeode()->getBoundingBox().yMin(), terrain->asGeode()->getBoundingBox().yMax());
    std::uniform_real_distribution<float> disTaille(5, 20);


    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    texture->setImage(osgDB::readImageFile("arbre.tga"));
    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);


    osg::ref_ptr<osg::Group> foret = new osg::Group;

    for (size_t i = 0; i < nb_arbres; ++i) {
        float taille = disTaille(gen);

        osg::ref_ptr<osg::Geometry> quad = osg::createTexturedQuadGeometry({-5, 0, 0}, {taille, 0, 0}, {0, 0, taille}, 0, 0, 1, 1);
        quad->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
        quad->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
        quad->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

        osg::ref_ptr<osg::Billboard> billboard = new osg::Billboard;
        billboard->setMode(osg::Billboard::AXIAL_ROT);
        billboard->setAxis({0, 0, 1});
        billboard->addDrawable(quad);
        billboard->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

        osg::Vec3 pos, normale;
        intersection_terrain(disX(gen), disY(gen), terrain, pos, normale);

        osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
        pat->setPosition(pos);
        pat->addChild(billboard);
        foret->addChild(pat);
    }

    return foret.release();
}

osg::Group * creation_foret(osg::Node * terrain, const std::string &fileName)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> disTaille(5, 20);

    osg::ref_ptr<osg::Image> image = osgDB::readImageFile(fileName);

    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    texture->setImage(osgDB::readImageFile("arbre.tga"));
    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);

    osg::ref_ptr<osg::Group> foret = new osg::Group;

    for (size_t y = 0; y < image->t(); ++y) {
        for (size_t x = 0; x < image->s(); ++x) {
            if (*image->data(x, y) != 0) {
                float taille = disTaille(gen);

                osg::ref_ptr<osg::Geometry> quad = osg::createTexturedQuadGeometry({-5, 0, 0}, {taille, 0, 0}, {0, 0, taille}, 0, 0, 1, 1);
                quad->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
                quad->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
                quad->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

                osg::ref_ptr<osg::Billboard> billboard = new osg::Billboard;
                billboard->setMode(osg::Billboard::AXIAL_ROT);
                billboard->setAxis({0, 0, 1});
                billboard->addDrawable(quad);
                billboard->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

                osg::Vec3 pos, normale;
                intersection_terrain(x/float(image->s())*terrain->asGeode()->getBoundingBox().xMax(), y/float(image->t())*terrain->asGeode()->getBoundingBox().yMax(), terrain, pos, normale);

                osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
                pat->setPosition(pos);
                pat->addChild(billboard);
                foret->addChild(pat);
            }
        }
    }

    return foret.release();
}

osg::Node * creation_tank(osg::Node * terrain)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> disX(terrain->asGeode()->getBoundingBox().xMin(), terrain->asGeode()->getBoundingBox().xMax());
    std::uniform_real_distribution<float> disY(terrain->asGeode()->getBoundingBox().yMin(), terrain->asGeode()->getBoundingBox().yMax());

    osg::ref_ptr<osg::Group> scene = new osg::Group;

    osg::ref_ptr<osg::Node> tank = osgDB::readNodeFile("t72-tank/t72-tank_des.flt");

    osg::Vec3 pos, normale;
    intersection_terrain(disX(gen), disY(gen), terrain, pos, normale);

    osg::Quat rotation;
    rotation.makeRotate({0, 0, 1}, normale);

    osg::ref_ptr<osg::AnimationPath> ap = new osg::AnimationPath;
    ap->setLoopMode(osg::AnimationPath::SWING);

    const size_t nb_points = 2;
    for (size_t i = 0; i < nb_points; ++i) {
        ap->insert(i/double(nb_points)*10, osg::AnimationPath::ControlPoint({disX(gen), disY(gen), 0}));
    }

    osg::ref_ptr<osg::AnimationPathCallback> apc = new AnimationTank(ap, terrain, scene);

    osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
    pat->setName("tank_pat");
    pat->setPosition(pos);
    pat->setAttitude(rotation);
    pat->addChild(tank);
    pat->setUpdateCallback(apc);

    osg::ref_ptr<osgParticle::SmokeEffect> smoke = new osgParticle::SmokeEffect;
    smoke->setName("tank_smoke");
    smoke->setTextureFileName("smoke.png");
    smoke->setIntensity(2);
    smoke->setScale(4);
    smoke->setPosition({50, 50, 50});
    smoke->setEmitterDuration(9999999);

    scene->addChild(pat);
    scene->addChild(smoke);

    return scene.release();
}

int main(int argc, char *argv[])
{
    osg::DisplaySettings::instance()->setNumMultiSamples(8);

    osg::ref_ptr<osg::Node> terrain = creation_terrain();

    osg::ref_ptr<osg::LightSource> light1 = new osg::LightSource;
    light1->getLight()->setLightNum(1);
    light1->getLight()->setPosition({1, 1, 1, 0});
    light1->getLight()->setAmbient({0.5, 0.5, 0.5, 1});
    light1->getLight()->setDiffuse({1, 1, 1, 1});
    light1->getLight()->setSpecular({0.2, 0.2, 0.2, 1});


    osg::ref_ptr<osg::Group> lights = new osg::Group;
    lights->addChild(light1);

    osg::ref_ptr<osgShadow::SoftShadowMap> st = new osgShadow::SoftShadowMap;
    st->setSoftnessWidth(0.001);

    osg::ref_ptr<osgShadow::ShadowedScene> shadowedScene = new osgShadow::ShadowedScene;
    shadowedScene->setShadowTechnique(st);
    shadowedScene->addChild(lights);
    shadowedScene->addChild(terrain);
    shadowedScene->addChild(creation_foret(terrain, "foret.tga"));
    shadowedScene->addChild(creation_tank(terrain));

    osg::ref_ptr<osg::Group> scene = new osg::Group;
    scene->addChild(shadowedScene);

    osg::ref_ptr<osg::StateSet> state = scene->getOrCreateStateSet();
    state->setMode(GL_LIGHT0, osg::StateAttribute::OFF);
    state->setMode(GL_LIGHT1, osg::StateAttribute::ON);

    osg::ref_ptr<osg::Camera> cam1 = new osg::Camera;
    cam1->getOrCreateStateSet()->setGlobalDefaults();
    cam1->setClearColor({0, 0, 0, 1});
    cam1->setProjectionMatrixAsPerspective(30, 4.f/3.f, 0.1, 1000);
    cam1->setViewMatrixAsLookAt({0, -10, 10}, {0, 0, 0}, {0, 0, 1});

    osg::ref_ptr<EventHandler> eventHandler = new EventHandler(state, scene);

    osgViewer::Viewer viewer;
    viewer.setCamera(cam1);
    viewer.setSceneData(scene);
    viewer.addEventHandler(eventHandler);
    viewer.addEventHandler(new osgViewer::StatsHandler);
    viewer.setRunMaxFrameRate(30.0);
    viewer.setUpViewInWindow(100, 50, 800, 600);

    osgViewer::Viewer::Windows windows;
    viewer.getWindows(windows);
    windows[0]->setWindowName("TP 5");

    return viewer.run();
}
