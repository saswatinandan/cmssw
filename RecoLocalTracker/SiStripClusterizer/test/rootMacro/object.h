#pragma once

struct Track{

        int idx;
        float pt;
        float eta;
        float phi;
        int   hits;
        float mass;
        Track(): idx(-1),
        pt(0), eta(0), phi(0), hits(0), mass(0)
        {};

        Track(int& in_idx,
            const float& in_pt,
            const float& in_eta,
            const float& in_phi,
            const int& in_hits,
	    const float mass=0
           ):
           idx(in_idx),
           pt(in_pt),
           eta(in_eta),
           phi(in_phi),
           hits(in_hits)
        {};
	void set_mass(const float& in_mass) { mass = in_mass;};

};
