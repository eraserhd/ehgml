#ifndef CModel_hh
#define CModel_hh

#include "CObject.hh"
#include "CClosure.hh"
#include "CMatrix.hh"
#include "CRay.hh"
#include "CLight.hh"
#include "CHitInfo.hh"

#pragma interface

//////////////////////////////////////////////////////////////////////////////
// CModel
class CModel : public CObject {
    friend class CHitInfo;
public:
    static const char *type_name;
protected:
    CClosure *	    surf;
    CMatrix	    transf;
    bool	    do_csg_hits;

    CPoint	    r_origin;
    CMatrix	    r_itransf;

public:
    CModel (CClosure *_surf);
    CModel (const CModel& rhs);

    virtual ~CModel ();
    virtual std::string perty_name () const = 0;

    CModel *translate (double x, double y, double z) const;
    CModel *scale     (double x, double y, double z) const;
    CModel *rotatex   (double deg) const;
    CModel *rotatey   (double deg) const;
    CModel *rotatez   (double deg) const;

    virtual void render_prep ();
    virtual bool hit_test (CHitInfo& pt) = 0;
    virtual bool owns (const CModel *model) const;

    virtual void mark (void);
    virtual void enable_csg_hits (void);

    CPoint point_to_local (const CPoint& pt) const;
    CPoint point_to_world (const CPoint& pt) const;
    CPoint vector_to_local (const CPoint& vic) const;
    CPoint vector_to_world (const CPoint& vic) const;

protected:
    virtual CModel *copy (void) const = 0;
    inline void transform (const CMatrix& matr)
    { transf *= matr; }

    void make_refl_angle (CHitInfo& info, const CPoint& normal);

    //FIXME: Move this to somewhere else.
    static int quadratic_solve (double a, double b, double c,
	double& d1, double& d2);
};

typedef TObjectRef<CModel> CModelRef;

inline std::ostream& operator << (std::ostream& o, const CModel& rhs)
{
    o <<rhs.perty_name ()<<std::endl;
    return o;
}

//////////////////////////////////////////////////////////////////////////////
// CPlaneModel
class CPlaneModel : public CModel {
protected:
    CPoint	r_normal;

public:
    CPlaneModel (CClosure *_surf);
    CPlaneModel (const CPlaneModel& rhs);
    virtual ~CPlaneModel ();

    virtual std::string perty_name () const;
    virtual void render_prep ();
    virtual bool hit_test (CHitInfo& pt);
//    virtual bool shadow_hit_test (const CLightInfo& info);

protected:
    virtual CModel *copy () const;
};

//////////////////////////////////////////////////////////////////////////////
// CSphereModel
class CSphereModel : public CModel {
public:
    CSphereModel (CClosure *_surf);
    CSphereModel (const CSphereModel& rhs);
    virtual ~CSphereModel ();

    virtual std::string perty_name () const;
    virtual void render_prep ();
    virtual bool hit_test (CHitInfo& pt);
//    virtual bool shadow_hit_test (const CLightInfo& info);

protected:
    virtual CModel *copy () const;
};

//////////////////////////////////////////////////////////////////////////////
// CCubeModel (Tier-2)
class CCubeModel : public CModel {
public:
    CCubeModel (CClosure *_surf);
    CCubeModel (const CCubeModel& rhs);
    virtual ~CCubeModel ();

    virtual std::string perty_name () const;
    virtual void render_prep ();
    virtual bool hit_test (CHitInfo& hit);
//    virtual bool shadow_hit_test (const CLightInfo& info);

protected:
    virtual CModel *copy () const;
    void hit_face (int& count, CHitInfo& our_hit, CRay& ray, int face);
};

//////////////////////////////////////////////////////////////////////////////
// CConeModel (Tier-2)
class CConeModel : public CModel {
public:
    CConeModel (CClosure *_surf);
    CConeModel (const CConeModel& rhs);
    virtual ~CConeModel ();

    virtual std::string perty_name () const;
    virtual void render_prep ();
    virtual bool hit_test (CHitInfo& hit);
//    virtual bool shadow_hit_test (const CLightInfo& info);

protected:
    virtual CModel *copy () const;
};

//////////////////////////////////////////////////////////////////////////////
// CCylinderModel (Tier-2)
class CCylinderModel : public CModel {
public:
    CCylinderModel (CClosure *_surf);
    CCylinderModel (const CCylinderModel& rhs);
    virtual ~CCylinderModel ();

    virtual std::string perty_name () const;
    virtual void render_prep ();
    virtual bool hit_test (CHitInfo& hit);

protected:
    virtual CModel *copy () const;
};

//////////////////////////////////////////////////////////////////////////////
// CCompositeModel - base class for union, difference, and intersection 
//                   models (still abstract, tho).
class CCompositeModel: public CModel {
protected:
    CModel *model1, *model2;

public:
    CCompositeModel (CModel *_model1, CModel *_model2);
    CCompositeModel (const CCompositeModel& rhs);
    virtual ~CCompositeModel (void);

    virtual void render_prep (void);
    virtual bool owns (const CModel *model) const;
    virtual void mark (void);
    virtual void enable_csg_hits (void);
};

//////////////////////////////////////////////////////////////////////////////
// CDifferenceModel (Tier-3)
class CDifferenceModel : public CCompositeModel {
public:
    CDifferenceModel (CModel *_model1, CModel *model2);
    CDifferenceModel (const CDifferenceModel& rhs);
    virtual ~CDifferenceModel (void);

    virtual std::string perty_name (void) const;
    virtual bool hit_test (CHitInfo& info);
    virtual void render_prep (void);

protected:
    virtual CModel *copy (void) const;
};

//////////////////////////////////////////////////////////////////////////////
// CIntersectionModel (Tier-3)
class CIntersectionModel : public CCompositeModel {
public:
    CIntersectionModel (CModel *_model1, CModel *_model2);
    CIntersectionModel (const CIntersectionModel& rhs);
    virtual ~CIntersectionModel (void);

    virtual std::string perty_name () const;
    virtual bool hit_test (CHitInfo& pt);
    virtual void render_prep (void);

protected:
    virtual CModel *copy () const;
};

//////////////////////////////////////////////////////////////////////////////
// CUnionModel
class CUnionModel : public CCompositeModel {
public:
    CUnionModel (CModel *_model1, CModel *_model2);
    CUnionModel (const CUnionModel& rhs);
    virtual ~CUnionModel ();

    virtual std::string perty_name () const;
    virtual bool hit_test (CHitInfo& pt);

protected:
    virtual CModel *copy () const;
};

#endif // CModel_hh

// vi:set sts=4 sw=4:
