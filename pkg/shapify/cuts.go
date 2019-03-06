package shapify

// a cut is a cut point and its length, they improve genetic operators
// efficiency by using domain specific knowledge, cut points are not chosen
// totally at random because they are defined at specific,
// representation-specific indices.
type cut struct{ index, length uint }

// a cutset is the set of cut points on which to apply genetic operators
type cutset struct {
	wholetris []cut // whole triangles (color + vertices)
	tris      []cut // triangles (vertices only)
	colors    []cut // colors (background + triangles)
}

func (cs *cutset) set(cfg Config) {
	nbpoint := cfg.hbits + cfg.wbits // number of bits per point
	nbtri := 3*nbpoint + nbpcolor    // number of bits per triangle

	// whole triangle cuts (color included)
	for i := uint(0); i < uint(cfg.Ntris); i++ {
		cs.wholetris = append(cs.wholetris, cut{nbheader + i*nbtri, nbtri})
	}

	// triangle cuts (3 vertices only)
	for i := uint(0); i < uint(cfg.Ntris); i++ {
		cs.tris = append(cs.tris, cut{nbheader + i*nbtri + nbpcolor, 3 * nbpoint})
	}

	// color cuts
	cs.colors = append(cs.colors, cut{0, nbheader}) // background color (whole header)
	for i := uint(0); i < uint(cfg.Ntris); i++ {
		cs.colors = append(cs.colors, cut{nbheader + i*nbtri, nbpcolor})
	}
}


