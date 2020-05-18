serialize EntranceZone@ entranceZone;
serialize int why = 17;
serialize Vector3f whVy = Vector3f(17, 14, 14);
serialize string uh = "aaah";

external shared enum RoomType;

void main() {
    @entranceZone = EntranceZone();
    entranceZone.registerRoom("hll_plain_4", Room4, hll_plain_4::construct);
    entranceZone.generate();
    PerFrame::register(render);
}

void render(float interpolation) {
    entranceZone.render(interpolation);
}