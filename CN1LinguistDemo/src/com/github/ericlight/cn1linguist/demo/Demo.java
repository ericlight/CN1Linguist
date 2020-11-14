package com.github.ericlight.cn1linguist.demo;


import static com.codename1.ui.CN.*;
import com.codename1.ui.Form;
import com.codename1.ui.Dialog;
import com.codename1.ui.plaf.UIManager;
import com.codename1.ui.util.Resources;
import com.codename1.l10n.L10NManager;
import com.codename1.ui.Toolbar;
import com.codename1.ui.layouts.BoxLayout;
import com.codename1.ui.Button;
import com.codename1.ui.TextField;
import com.github.ericlight.cn1linguist.CL;
import static com.github.ericlight.cn1linguist.CL.tr;
import java.util.Map;

/**
 * This file was generated by <a href="https://www.codenameone.com/">Codename One</a> for the purpose 
 * of building native mobile applications using Java.
 */
public class Demo {

    private Form current;
    private Resources theme;

    public void init(Object context) {
        // use two network threads instead of one
        updateNetworkThreadCount(2);

        theme = UIManager.initFirstTheme("/theme");

        // Enable Toolbar on all Forms by default
        Toolbar.setGlobalToolbar(true);

        String lang=L10NManager.getInstance().getLanguage();
        if(lang.equals(CL.FRENCH_LANG)){
            Map m=theme.getL10N("myBundle", CL.FRENCH_LANG);
            UIManager.getInstance().setBundle(m);
        }
    }
    
    public void start() {
        if(current != null){
            current.show();
            return;
        }
        
        Form f = new Form(tr("Welcome"), BoxLayout.y());
                     
        TextField username=new TextField();        
        TextField password=new TextField();
        password.setConstraint(TextField.PASSWORD);
        
        Button register=new Button(tr("Connect"));

        f.add(tr("Username")); 
        f.add(username);
        f.add(tr("Password"));
        f.add(password);
        f.add(register);

        f.show();
    }

    public void stop() {
        current = getCurrentForm();
        if(current instanceof Dialog) {
            ((Dialog)current).dispose();
            current = getCurrentForm();
        }
    }
    
    public void destroy() {
    }

}
