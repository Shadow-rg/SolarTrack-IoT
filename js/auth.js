function login(){

    const username =
    document.getElementById("username").value;

    const password =
    document.getElementById("password").value;

    const errorMessage =
    document.getElementById("errorMessage");

    if(
        username === "admin" &&
        password === "solartrack2026"
    ){

        localStorage.setItem(
            "solartrackAuth",
            "true"
        );

        window.location.href =
        "dashboard.html";

    }else{

        errorMessage.classList.remove("hidden");
    }
}