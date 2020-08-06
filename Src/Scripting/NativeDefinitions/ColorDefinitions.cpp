#include "ColorDefinitions.h"

#include <Color/Color.h>

static void colorConstructor(void* memory) {
    new(memory) PGE::Color();
}

static void colorConstructorParametrized(float r, float g, float b, float a, void* memory) {
    new(memory) PGE::Color(r, g, b, a);
}

static void colorConstructorParametrizedInt(int r, int g, int b, int a, void* memory) {
    new(memory) PGE::Color(r, g, b, a);
}

static void colorDestructor(void* memory) {
    ((PGE::Color*)memory)->~Color();
}

ColorDefinitions::ColorDefinitions(ScriptManager* mgr) {
    engine = mgr->getAngelScriptEngine();

    engine->RegisterObjectType("Color", sizeof(PGE::Color), asOBJ_VALUE | asOBJ_APP_CLASS_ALLFLOATS | asGetTypeTraits<PGE::Color>());
    engine->RegisterObjectBehaviour("Color", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(colorConstructor), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectBehaviour("Color", asBEHAVE_CONSTRUCT, "void f(float r, float g, float b, float a=1.0)", asFUNCTION(colorConstructorParametrized), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectBehaviour("Color", asBEHAVE_CONSTRUCT, "void f(int r, int g, int b, int a=1.0)", asFUNCTION(colorConstructorParametrizedInt), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectBehaviour("Color", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(colorDestructor), asCALL_CDECL_OBJLAST);

    engine->RegisterObjectProperty("Color", "float red", asOFFSET(PGE::Color, red));
    engine->RegisterObjectProperty("Color", "float green", asOFFSET(PGE::Color, green));
    engine->RegisterObjectProperty("Color", "float blue", asOFFSET(PGE::Color, blue));
    engine->RegisterObjectProperty("Color", "float alpha", asOFFSET(PGE::Color, alpha));

    engine->RegisterObjectMethod("Color", "int get_redInt() property", asMETHOD(PGE::Color, getRedInt), asCALL_THISCALL);
    engine->RegisterObjectMethod("Color", "void set_redInt(int r) property", asMETHOD(PGE::Color, setRedInt), asCALL_THISCALL);
    engine->RegisterObjectMethod("Color", "int get_greenInt() property", asMETHOD(PGE::Color, getGreenInt), asCALL_THISCALL);
    engine->RegisterObjectMethod("Color", "void set_greenInt(int g) property", asMETHOD(PGE::Color, setGreenInt), asCALL_THISCALL);
    engine->RegisterObjectMethod("Color", "int get_blueInt() property", asMETHOD(PGE::Color, getBlueInt), asCALL_THISCALL);
    engine->RegisterObjectMethod("Color", "void set_blueInt(int b) property", asMETHOD(PGE::Color, setBlueInt), asCALL_THISCALL);
    engine->RegisterObjectMethod("Color", "int get_alphaInt() property", asMETHOD(PGE::Color, getAlphaInt), asCALL_THISCALL);
    engine->RegisterObjectMethod("Color", "void set_alphaInt(int a) property", asMETHOD(PGE::Color, setAlphaInt), asCALL_THISCALL);

    engine->RegisterObjectMethod("Color", "Color& opAssign(const Color &in other)", asMETHODPR(PGE::Color, operator=, (const PGE::Color&), PGE::Color&), asCALL_THISCALL);
}
