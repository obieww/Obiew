package com.obiew.Controllers;

import com.obiew.Entities.Like;
import com.obiew.Entities.Obiew;
import com.obiew.Repositories.UserRepository;
import com.obiew.Entities.User;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.*;

import java.util.List;


@Controller
public class LoadBalanceController {
    @Autowired
    private UserRepository userRepository;

    @RequestMapping("/register")
    public @ResponseBody User register(@RequestParam String username, @RequestParam String password) {
        User user = new User(username, password);
        userRepository.save(user);
        return user;
    }

    @RequestMapping("/alluser")
    public @ResponseBody ResponseEntity<Iterable<User>> alluser() {
        return new ResponseEntity<Iterable<User>>(userRepository.findAll(), HttpStatus.OK);
    }

    @RequestMapping("/run")
    public @ResponseBody ResponseEntity<User> run() {
        User user = new User("aaa", "123");
        Obiew obiew = new Obiew("first obiew");
        obiew.addComment(new Obiew("comment"));
        user.addObiew(obiew);
        user.addObiew(new Obiew("second obiew"));
        return new ResponseEntity<>(user, HttpStatus.OK);
    }
}
