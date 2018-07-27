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
    EventHandler(osg::ref_ptr<osg::StateSet> state, osg::ref_ptr<osg::Group> scene, osg::ref_ptr<osg::AnimationPathCallback> bateau_anim) : _state(state), _scene(scene), _bateau_anim(bateau_anim), _timeMultiplier(1.0), _explosed(false) {
        SearchNode searchBateauPat("bateau_pat");
        _scene->accept(searchBateauPat);
        _bateau_pat = dynamic_cast<osg::PositionAttitudeTransform*>(searchBateauPat.node());
    }

    virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa) {
        switch (ea.getEventType()) {
        case osgGA::GUIEventAdapter::KEYDOWN:
            switch (ea.getKey()) {
            case 'p':
                _bateau_anim->setPause(!_bateau_anim->getPause());
                break;
            case '+':
                _timeMultiplier += 0.1;
                _bateau_anim->setTimeMultiplier(_timeMultiplier);
                break;
            case '-':
                _timeMultiplier -= 0.1;
                _bateau_anim->setTimeMultiplier(_timeMultiplier);
                break;
            case 'e':
                explosion();
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
    void explosion() {
        if (!_explosed) {
            osg::ref_ptr<osgParticle::SmokeEffect> smoke = new osgParticle::SmokeEffect;
            smoke->setName("bateau_smoke");
            smoke->setTextureFileName("smoke.png");
            smoke->setIntensity(2);
            smoke->setScale(4);
            smoke->setEmitterDuration(9999999);

            osg::ref_ptr<osgParticle::ExplosionEffect> expl = new osgParticle::ExplosionEffect;
            expl->setTextureFileName("fire.png");
            expl->setIntensity(2);
            expl->setScale(4);
            expl->setPosition(_bateau_pat->getPosition());

            _scene->addChild(smoke);
            _scene->addChild(expl);
            _explosed = true;
        }
    }


private:
    osg::ref_ptr<osg::StateSet> _state;
    osg::ref_ptr<osg::Group> _scene;
    osg::ref_ptr<osg::PositionAttitudeTransform> _bateau_pat;
    osg::ref_ptr<osg::AnimationPathCallback> _bateau_anim;
    double _timeMultiplier;
    bool _explosed;
};

osg::ref_ptr<osg::Node> creation_sol()
{
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    texture->setImage(osgDB::readImageFile("mer.jpg"));
    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

    osg::Material* mat = new osg::Material;
    mat->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.4, 0.4, 0.4, 1.0));
    mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.9, 0.9, 0.9, 1.0));
    mat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.4, 0.4, 0.4, 1.0));
    mat->setShininess(osg::Material::FRONT_AND_BACK, 64);

    osg::ref_ptr<osg::Geometry> quad = osg::createTexturedQuadGeometry({0.f, 0.f, 0.f}, {200.f, 0.f, 0.f}, {0.f, 200.f, 0.f}, 0.f, 0.f, 10.f, 10.f);
    quad->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
    quad->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
    quad->getOrCreateStateSet()->setAttributeAndModes(mat);

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(quad);

    osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
    pat->setPosition({-100.0, -100.0, 0.0});
    pat->addChild(geode);

    return pat;
}

class AnimationBateau : public osg::AnimationPathCallback
{
public:
    AnimationBateau(osg::AnimationPath * p, osg::Node * bateau) : osg::AnimationPathCallback(p), _bateau(bateau) {}

    virtual void operator ()(osg::Node * n, osg::NodeVisitor * nv) {
        osg::AnimationPathCallback::operator ()(n, nv);
        osg::PositionAttitudeTransform * transform = dynamic_cast<osg::PositionAttitudeTransform*>(n);
        if (transform) {
            osg::Vec3f pos = transform->getPosition();

            SearchNode searchSmoke("bateau_smoke");
            _bateau->accept(searchSmoke);
            osgParticle::ParticleEffect * smoke = dynamic_cast<osgParticle::ParticleEffect*>(searchSmoke.node());
            if (smoke)
                smoke->setPosition(pos);
        }
    }

private:
    osg::Node * _bateau;
};

osg::ref_ptr<osg::Group> creation_bateau(osg::ref_ptr<osg::AnimationPathCallback> &bateau_anim)
{
    osg::ref_ptr<osg::Group> scene = new osg::Group;

    osg::ref_ptr<osg::Node> bateau = osgDB::readNodeFile("bateau.3ds");
    bateau->setName("bateau_bateau");

    osg::ref_ptr<osg::AnimationPath> ap = new osg::AnimationPath;
    ap->setLoopMode(osg::AnimationPath::LOOP);

    const size_t nb_points = 100;
    const osg::Vec2d center(0, 0);
    const double radius = 50;
    double t = 0.0;
    for (size_t i = 0; i < nb_points; ++i) {
        ap->insert(i/double(nb_points)*100, osg::AnimationPath::ControlPoint({center.x() + std::cos(t) * radius, center.y() + std::sin(t) * radius, 0}, osg::Quat(t + M_PI, osg::Vec3d{0.0, 0.0, 1.0})));
        t += M_PI * 2 / (nb_points - 1);
    }

    osg::ref_ptr<osg::AnimationPathCallback> apc = new AnimationBateau(ap, scene);
    apc->setName("bateau_animation");
    bateau_anim = apc;

    osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
    pat->setName("bateau_pat");
    pat->addChild(bateau);
    pat->setUpdateCallback(apc);

    scene->addChild(pat);

    return scene;
}

osg::ref_ptr<osg::Node> creation_statue() {
    osg::ref_ptr<osg::Node> statue = osgDB::readNodeFile("statue.3ds");

    osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
    pat->setPosition({0.0, 0.0, 0.0});
    pat->addChild(statue);

    return pat;
}

int main(int argc, char *argv[])
{
    osg::DisplaySettings::instance()->setNumMultiSamples(8);

    osg::ref_ptr<osg::LightSource> light1 = new osg::LightSource;
    light1->getLight()->setLightNum(1);
    light1->getLight()->setPosition({1, -1, 1, 0});
    light1->getLight()->setAmbient({0.4, 0.4, 0.4, 1});
    light1->getLight()->setDiffuse({0.9, 0.9, 0.9, 1});
    light1->getLight()->setSpecular({0.2, 0.2, 0.2, 1});


    osg::ref_ptr<osg::Group> lights = new osg::Group;
    lights->addChild(light1);

    osg::ref_ptr<osgShadow::ShadowMap> st = new osgShadow::ShadowMap;

    osg::ref_ptr<osg::AnimationPathCallback> bateau_anim;
    osg::ref_ptr<osg::Group> bateau = creation_bateau(bateau_anim);

    osg::ref_ptr<osgShadow::ShadowedScene> shadowedScene = new osgShadow::ShadowedScene;
    shadowedScene->setShadowTechnique(st);
    shadowedScene->addChild(lights);
    shadowedScene->addChild(creation_sol());
    shadowedScene->addChild(creation_statue());
    shadowedScene->addChild(bateau);

    osg::ref_ptr<osgParticle::PrecipitationEffect> precip = new osgParticle::PrecipitationEffect;
    precip->setWind({0, 0, -1});
    precip->setParticleSpeed(0.4);
    precip->rain(0.3);

    osg::ref_ptr<osg::Group> scene = new osg::Group;
    scene->addChild(shadowedScene);
    scene->addChild(precip);

    osg::ref_ptr<osg::StateSet> state = scene->getOrCreateStateSet();
    state->setMode(GL_LIGHT0, osg::StateAttribute::OFF);
    state->setMode(GL_LIGHT1, osg::StateAttribute::ON);

    osg::ref_ptr<EventHandler> eventHandler = new EventHandler(state, bateau, bateau_anim);

    SearchNode searchBateau("bateau_bateau");
    bateau->accept(searchBateau);

    osg::ref_ptr<osgGA::NodeTrackerManipulator> manip = new osgGA::NodeTrackerManipulator;
    manip->setTrackNode(searchBateau.node());
    manip->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER);

    osgViewer::Viewer viewer;
    viewer.setSceneData(scene);
    viewer.addEventHandler(eventHandler);
    viewer.addEventHandler(new osgViewer::StatsHandler);
    viewer.setRunMaxFrameRate(30.0);
    viewer.setUpViewInWindow(100, 50, 1000, 600);
    viewer.getCamera()->setClearColor({0.8, 0.8, 0.8, 1});
    viewer.setCameraManipulator(manip);

    osgViewer::Viewer::Windows windows;
    viewer.getWindows(windows);
    windows[0]->setWindowName("Loic Escales");

    return viewer.run();
}
