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

class EventHandler : public osgGA::GUIEventHandler
{
public:
    EventHandler(osg::ref_ptr<osg::StateSet> state, osg::ref_ptr<osg::Node> scene) : _state(state), _scene(scene) {}

    virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa) {
        switch (ea.getEventType()) {
        case osgGA::GUIEventAdapter::KEYDOWN:
            switch (ea.getKey()) {
            case 'v': {
                SearchNode search("switch");
                _scene->accept(search);
                auto n = search.node();
                if (n != nullptr) {
                    switchSwitch(n->asSwitch());
                }
            }
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
    void switchSwitch(osg::Switch * sw) {
        auto child = sw->getChild(0);
        sw->setChildValue(child, !sw->getChildValue(child));
    }

private:
    osg::ref_ptr<osg::StateSet> _state;
    osg::ref_ptr<osg::Node> _scene;
};

osg::Group* creation_troupeau(int nb_vaches, float taillex, float tailley)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> disX(0.f, taillex);
    std::uniform_real_distribution<float> disY(0.f, tailley);
    std::uniform_real_distribution<float> disDeg(0.f, 360.f);

    osg::ref_ptr<osg::Node> vache_low = osgDB::readNodeFile("cow_low.3ds");
    osg::ref_ptr<osg::Node> vache_mid = osgDB::readNodeFile("cow_mid.3ds");
    osg::ref_ptr<osg::Node> vache_high = osgDB::readNodeFile("cow_high.3ds");

    osg::ref_ptr<osg::LOD> lod = new osg::LOD;
    lod->setRangeMode(osg::LOD::DISTANCE_FROM_EYE_POINT);
    lod->addChild(vache_high, 0, 10);
    lod->addChild(vache_mid, 10, 20);
    lod->addChild(vache_low, 20, 1000);

    osg::ref_ptr<osg::Group> vaches = new osg::Group;
    for (size_t i = 0; i < nb_vaches; ++i) {
        osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
        pat->setPosition({disX(gen), disY(gen), 0});
        pat->setAttitude(osg::Quat(osg::DegreesToRadians(disDeg(gen)), osg::Vec3{0.f, 0.f, 1.f}));
        pat->addChild(lod);
        vaches->addChild(pat);
    }
    return vaches.release();
}

osg::Node* creation_sol(float taillex, float tailley)
{
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    texture->setImage(osgDB::readImageFile("herbe.jpg"));
    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

    osg::ref_ptr<osg::Geometry> quad = osg::createTexturedQuadGeometry({0.f, 0.f, 0.f}, {taillex, 0.f, 0.f}, {0.f, tailley, 0.f}, 0.f, 0.f, 1.f, 1.f);
    quad->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
    quad->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(quad);

    return geode.release();
}

int main(int argc, char *argv[])
{
    osg::DisplaySettings::instance()->setNumMultiSamples(8);

    osg::ref_ptr<osg::Group> vaches = creation_troupeau(1000, 100, 100);

    osg::ref_ptr<osg::Switch> switchNode = new osg::Switch;
    switchNode->setName("switch");
    switchNode->addChild(vaches);

    osg::ref_ptr<osg::Node> sol = creation_sol(100, 100);

    osg::ref_ptr<osg::LightSource> light1 = new osg::LightSource;
    light1->getLight()->setLightNum(1);
    light1->getLight()->setPosition({0, 0, 1, 0});
    light1->getLight()->setAmbient({0.5, 0.5, 0.5, 1});
    light1->getLight()->setDiffuse({1, 1, 1, 1});
    light1->getLight()->setSpecular({0.2, 0.2, 0.2, 1});

    osg::ref_ptr<osg::Group> lights = new osg::Group;
    lights->addChild(light1);

    osg::ref_ptr<osg::Group> scene = new osg::Group;
    scene->addChild(lights);
    scene->addChild(switchNode);
    scene->addChild(sol);

    osg::ref_ptr<osg::StateSet> state = scene->getOrCreateStateSet();
    state->setMode(GL_LIGHT1, osg::StateAttribute::ON);

    osg::ref_ptr<osgGA::NodeTrackerManipulator> manip = new osgGA::NodeTrackerManipulator;
    manip->setTrackNode(vaches);
    manip->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER);

    osg::ref_ptr<EventHandler> eventHandler = new EventHandler(state, scene);

    osg::ref_ptr<osg::Camera> cam1 = new osg::Camera;
    cam1->getOrCreateStateSet()->setGlobalDefaults();
    cam1->setClearColor({0, 0, 0, 1});
    cam1->setProjectionMatrixAsPerspective(30, 4.f/3.f, 0.1, 1000);
    cam1->setViewMatrixAsLookAt({0, -10, 10}, {0, 0, 0}, {0, 0, 1});

    osgViewer::Viewer viewer;
    viewer.setCamera(cam1);
    viewer.setSceneData(scene);
    viewer.addEventHandler(eventHandler);
    viewer.addEventHandler(new osgViewer::StatsHandler);
    viewer.setRunMaxFrameRate(30.0);
    viewer.setUpViewInWindow(100, 50, 800, 600);

    osgViewer::Viewer::Windows windows;
    viewer.getWindows(windows);
    windows[0]->setWindowName("TP 2");

    return viewer.run();
}
