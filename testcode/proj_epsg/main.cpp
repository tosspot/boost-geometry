#include <QCoreApplication>

#include <QElapsedTimer>
#include <QDebug>
#include <proj.h>


int main(int argc, char *argv[])
{
    QElapsedTimer t;

    PJ_COORD a, b;
    auto *P = proj_create_crs_to_crs (nullptr, "EPSG:3857", "EPSG:4326", nullptr);
    a = proj_coord(-338501.42, 7065303.37, 0, 0);


    t.start();

    auto iters = 10000000;
    for (int i = 0; i < iters; ++i)
    {
       // qDebug() << "Cunt" << i;

        b = proj_trans(P, PJ_FWD, a);
        //printf("longitude: %g, latitude: %g\n", b.lp.lam, b.lp.phi);

    }

    printf("Elapsed %lld\n", t.nsecsElapsed() / iters);

    printf("longitude: %g, latitude: %g\n", b.lp.lam, b.lp.phi);


    proj_destroy(P);
    return 0;
}
